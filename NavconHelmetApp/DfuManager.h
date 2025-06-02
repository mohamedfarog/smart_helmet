#ifndef DFUMANAGER_H
#define DFUMANAGER_H

#include <QObject>
#include <QProcess>

class DfuManager : public QObject {
    Q_OBJECT
public:
    explicit DfuManager(QObject *parent = nullptr);

    Q_INVOKABLE void startDfu(const QString &filePath);

signals:
    void dfuOutput(QString text);
    void dfuFinished();

private slots:
    void readOutput();
    void readError();

private:
    QProcess mcumgrProcess;
};

#endif // DFUMANAGER_H
