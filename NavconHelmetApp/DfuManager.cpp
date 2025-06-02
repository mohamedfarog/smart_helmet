#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QRegularExpression>
#include <memory>
#include "DfuManager.h"

DfuManager::DfuManager(QObject *parent) : QObject(parent) {
    connect(&mcumgrProcess, &QProcess::readyReadStandardOutput, this, &DfuManager::readOutput);
    connect(&mcumgrProcess, &QProcess::readyReadStandardError, this, &DfuManager::readError);
    connect(&mcumgrProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int, QProcess::ExitStatus) {
        emit dfuFinished();
    });
}


void DfuManager::startDfu(const QString &filePath) {
//    Q_UNUSED(bleAddress);  // Using fixed peer_name

    const QString connString = "peer_name=NAVCON";
    const QString mcumgrPath = "/home/farog/go/bin/mcumgr";
    const QString hciIndex = "0";
    // BLE ADDRESS D9:B9:36:F8:43:F7

    QProcess hciCheck;
    hciCheck.start("hciconfig", QStringList());
    hciCheck.waitForFinished();
    QString output = QString::fromLocal8Bit(hciCheck.readAllStandardOutput());

    if (!output.contains("hci0")) {
        emit dfuOutput("❌ Bluetooth interface 'hci0' not found. Please check your Bluetooth adapter.\n");
        emit dfuFinished();
        return;
    }

    auto runCommand = [this, connString, mcumgrPath, hciIndex]
                      (const QStringList &args, const QString &stepName, std::function<void(QString)> onSuccess) {
        emit dfuOutput(QString("➡️ Running %1 with: %2\n").arg(stepName, args.join(" ")));

        QProcess *process = new QProcess(this);
        auto outputBuffer = std::make_shared<QString>();

        connect(process, &QProcess::readyReadStandardOutput, this, [this, process, outputBuffer]() {
            QString out = QString::fromLocal8Bit(process->readAllStandardOutput());
            *outputBuffer += out;
            emit dfuOutput(out);
        });

        connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
            emit dfuOutput(QString::fromLocal8Bit(process->readAllStandardError()));
        });

        connect(process, &QProcess::errorOccurred, this, [this, stepName](QProcess::ProcessError error) {
            emit dfuOutput(QString("❌ Error during %1: %2\n").arg(stepName).arg(error));
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this, process, stepName, onSuccess, outputBuffer](int exitCode, QProcess::ExitStatus exitStatus) {
            process->deleteLater();
            if (exitCode != 0 || exitStatus != QProcess::NormalExit) {
                emit dfuOutput(QString("❌ %1 failed with exit code %2\n").arg(stepName).arg(exitCode));
                emit dfuOutput(QString::fromLocal8Bit(process->readAllStandardError()));
                emit dfuFinished();
                 return;
            }
            emit dfuOutput(QString("✅ %1 finished successfully.\n").arg(stepName));
            onSuccess(*outputBuffer);
        });

        process->start(mcumgrPath, args);
    };

    // Step 1: Upload
    QStringList uploadArgs = {
        "--hci", hciIndex,
        "--conntype", "ble",
        "--connstring", connString,
        "image", "upload",
        "-t", "25",
        filePath
    };

    runCommand(uploadArgs, "DFU Upload", [=](const QString &) {
        // Step 2: List images
        QStringList listArgs = {
            "--hci", hciIndex,
            "--conntype", "ble",
            "--connstring", connString,
            "image", "list"
        };

        runCommand(listArgs, "DFU List", [=](const QString &listOutput) {
            QStringList blocks = listOutput.split(QRegularExpression(R"(\n\s*image=\d+\s+slot=\d+)"), QString::SkipEmptyParts);
            QString imageHash;

            for (const QString &block : blocks) {
                if (block.contains("bootable: true") &&
                    !block.contains("confirmed") &&
                    !block.contains("active")) {

                    QRegularExpression hashRe(R"(hash:\s*([a-fA-F0-9]+))");
                    QRegularExpressionMatch match = hashRe.match(block);
                    if (match.hasMatch()) {
                        imageHash = match.captured(1).trimmed();
                        break;
                    }
                }
            }

            if (imageHash.isEmpty()) {
                emit dfuOutput("❌ No suitable unconfirmed image found.\n");
                emit dfuFinished();
                return;
            }

            emit dfuOutput(QString("🆔 Using image hash for test: %1\n").arg(imageHash));

            // Step 3: Test image
            QStringList testArgs = {
                "--hci", hciIndex,
                "--conntype", "ble",
                "--connstring", connString,
                "image", "test",
                imageHash
            };

            runCommand(testArgs, "DFU Test", [=](const QString &) {
                // Step 4: Reset
                QStringList resetArgs = {
                    "--hci", hciIndex,
                    "--conntype", "ble",
                    "--connstring", connString,
                    "reset"
                };

                runCommand(resetArgs, "DFU Reset", [=](const QString &) {
                    emit dfuOutput("Waiting 40 seconds for reboot...\n");

                    // Step 5: Confirm with retries
                    int maxRetries = 3;
                    int delayBetweenRetriesMs = 10000;

                    auto tryConfirm = [=, this](int retryCount) mutable {
                        QStringList confirmArgs = {
                            "--hci", hciIndex,
                            "--conntype", "ble",
                            "--connstring", connString,
                            "image", "confirm"
                        };

                        runCommand(confirmArgs, QString("DFU Confirm (attempt %1)").arg(4 - retryCount), [=](const QString &) {
                            emit dfuOutput("🎉 DFU completed and confirmed successfully.\n");
                            emit dfuFinished();
                        });
                    };

                    QTimer::singleShot(40000, this, [=]() mutable {
                        std::function<void(int)> retryConfirm = nullptr; // declare first

                        retryConfirm = [=, this, &retryConfirm](int retryLeft) mutable {
                            if (retryLeft <= 0) {
                                emit dfuOutput("❌ DFU Confirm failed after all retries.\n");
                                emit dfuFinished();
                                return;
                            }
                            emit dfuOutput(QString("🔄 Trying DFU Confirm... attempts left: %1\n").arg(retryLeft));

                            QStringList confirmArgs = {
                                "--hci", hciIndex,
                                "--conntype", "ble",
                                "--connstring", connString,
                                "image", "confirm"
                            };

                            runCommand(confirmArgs, QString("DFU Confirm (attempt %1)").arg(4 - retryLeft), [=](const QString &) {
                                emit dfuOutput("🎉 DFU completed and confirmed successfully.\n");
                                emit dfuFinished();
                            });

                            if (retryLeft > 1) {
                                QTimer::singleShot(delayBetweenRetriesMs, this, [=]() mutable {
                                    retryConfirm(retryLeft - 1);
                                });
                            }
                        };

                        retryConfirm(maxRetries);
                    });
                });
            });
        });
    });
}



void DfuManager::readOutput() {
    emit dfuOutput(QString::fromLocal8Bit(mcumgrProcess.readAllStandardOutput()));
}

void DfuManager::readError() {
    emit dfuOutput(QString::fromLocal8Bit(mcumgrProcess.readAllStandardError()));
}
