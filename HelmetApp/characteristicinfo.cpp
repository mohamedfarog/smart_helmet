/***************************************************************************
**
** Copyright (C) 2013 BlackBerry Limited. All rights reserved.
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "characteristicinfo.h"
#include "qbluetoothuuid.h"
#include <QByteArray>
#include <QDebug>
#include<QString>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

#define AES_BLOCK_SIZE 16 // AES block size in bytes (128-bit)

#define AES_KEYLEN 32  // 256 bits for AES-256
CharacteristicInfo::CharacteristicInfo()
{
}


QByteArray CharacteristicInfo::encrypt(const QByteArray& plaintext, const QByteArray& key, QByteArray& iv)
{
    if (key.size() != 32) { // AES-256 requires 32-byte key
        qDebug() << "Invalid key size. Expected 32 bytes for AES-256.";
        return QByteArray();
    }

    // Ensure IV is provided (do not generate it)
       if (iv.isEmpty()) {
           qDebug() << "IV is empty. A valid IV must be provided.";
           return QByteArray();
       }


    // Create AES key
    AES_KEY aesKey;
    if (AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(key.constData()), 256, &aesKey) < 0) {
        qDebug() << "Failed to set encryption key.";
        return QByteArray();
    }

    int len = plaintext.size();
    int paddedLen = (len / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    QByteArray paddedPlaintext = plaintext;
    paddedPlaintext.resize(paddedLen);

    QByteArray ciphertext(paddedLen, '\0');
    AES_cbc_encrypt(reinterpret_cast<const unsigned char*>(paddedPlaintext.constData()),
                    reinterpret_cast<unsigned char*>(ciphertext.data()),
                    paddedPlaintext.size(), &aesKey, reinterpret_cast<unsigned char*>(iv.data()), AES_ENCRYPT);

    return ciphertext;
}


QByteArray CharacteristicInfo::decrypt(const QByteArray& ciphertext, const QByteArray& key, const QByteArray& iv)
{
    if (key.size() != 32) { // AES-256 requires 32-byte key
        qDebug() << "Invalid key size. Expected 32 bytes for AES-256.";
        return QByteArray();
    }

    // Create AES key
    AES_KEY aesKey;
    if (AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(key.constData()), 256, &aesKey) < 0) {
        qDebug() << "Failed to set decryption key.";
        return QByteArray();
    }

    int len = ciphertext.size();
    QByteArray decryptedText(len, '\0');

    AES_cbc_encrypt(reinterpret_cast<const unsigned char*>(ciphertext.constData()),
                    reinterpret_cast<unsigned char*>(decryptedText.data()),
                    len, &aesKey, reinterpret_cast<unsigned char*>(const_cast<char*>(iv.constData())), AES_DECRYPT);

    return decryptedText;
}


CharacteristicInfo::CharacteristicInfo(const QLowEnergyCharacteristic &characteristic):
    m_characteristic(characteristic)
{
}

void CharacteristicInfo::setCharacteristic(const QLowEnergyCharacteristic &characteristic)
{
    m_characteristic = characteristic;
    emit characteristicChanged();
}

QString CharacteristicInfo::getName() const
{

    //! [les-get-descriptors]
    QString name = m_characteristic.name();
    if (!name.isEmpty())
        return name;

    // find descriptor with CharacteristicUserDescription
    foreach (const QLowEnergyDescriptor &descriptor, m_characteristic.descriptors()) {
        if (descriptor.type() == QBluetoothUuid::CharacteristicUserDescription) {
            name = descriptor.value();
            break;
        }
    }
    //! [les-get-descriptors]

    if (name.isEmpty())
        name = "Navcon Data";

    return name;
}


QString CharacteristicInfo::getUuid() const
{
    const QBluetoothUuid uuid = m_characteristic.uuid();
    bool success = false;
    quint16 result16 = uuid.toUInt16(&success);
    if (success)
        return QStringLiteral("0x") + QString::number(result16, 16);

    quint32 result32 = uuid.toUInt32(&success);
    if (success)
        return QStringLiteral("0x") + QString::number(result32, 16);

    return uuid.toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
}



QString CharacteristicInfo::getValue() const
{

      // Pre-shared key (32 bytes for AES-256)
      QByteArray key = "6085F345F99C7D7AA157DCF6993E8F3D";
      QByteArray iv = "TH1515TH56A157DC";  // Pre-shared IV (must be 16 bytes for AES)

      // Decrypt
      // Show raw string first and hex value below
      QByteArray a = m_characteristic.value();
      QByteArray decryptedText = CharacteristicInfo::decrypt(a, key, iv);
      QString result;
      result += QLatin1Char('\n');
      result = decryptedText;


    if (a.isEmpty()) {
        result = QStringLiteral("<none>");
        //return result;

    }
    //result = a;
    qDebug() << "Decrypted Text:" << result;
    result += a.toHex();
    return result;

}

QString CharacteristicInfo::getHandle() const
{
    return QStringLiteral("0x") + QString::number(m_characteristic.handle(), 16);
}

QString CharacteristicInfo::getPermission() const
{
    QString properties = "( ";
    int permission = m_characteristic.properties();
    if (permission & QLowEnergyCharacteristic::Read)
        properties += QStringLiteral(" Read");
    if (permission & QLowEnergyCharacteristic::Write)
        properties += QStringLiteral(" Write");
    if (permission & QLowEnergyCharacteristic::Notify)
        properties += QStringLiteral(" Notify");
    if (permission & QLowEnergyCharacteristic::Indicate)
        properties += QStringLiteral(" Indicate");
    if (permission & QLowEnergyCharacteristic::ExtendedProperty)
        properties += QStringLiteral(" ExtendedProperty");
    if (permission & QLowEnergyCharacteristic::Broadcasting)
        properties += QStringLiteral(" Broadcast");
    if (permission & QLowEnergyCharacteristic::WriteNoResponse)
        properties += QStringLiteral(" WriteNoResp");
    if (permission & QLowEnergyCharacteristic::WriteSigned)
        properties += QStringLiteral(" WriteSigned");
    properties += " )";
    return properties;
}

QLowEnergyCharacteristic CharacteristicInfo::getCharacteristic() const
{
    return m_characteristic;
}
