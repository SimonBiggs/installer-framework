/**************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
**************************************************************************/

#include "init.h"
#include "updateoperations.h"

#include <QDir>
#include <QObject>
#include <QTest>
#include <QFile>
#include <QTextStream>

using namespace KDUpdater;
using namespace QInstaller;

class tst_mkdiroperationtest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
       QInstaller::init();
       QString path = QDir::current().path() + QDir::toNativeSeparators("/test");
       if (QDir(path).exists()) {
           QFAIL("Remove test folder first!");
       }
    }

    void testMissingArguments()
    {
        MkdirOperation op;

        QVERIFY(op.testOperation());
        QVERIFY(!op.performOperation());

        QCOMPARE(UpdateOperation::Error(op.error()), UpdateOperation::InvalidArguments);
        QCOMPARE(op.errorString(), QString("Invalid arguments in Mkdir: "
                                           "0 arguments given, exactly 1 arguments expected."));

    }

    void testCreateDirectory_data()
    {
         QTest::addColumn<QString>("directory");
         QTest::newRow("/test") << "/test";
         QTest::newRow("/test/test") << "/test/test";
         QTest::newRow("/test/test/test") << "/test/test/test";
    }

    void testCreateDirectory()
    {
        QFETCH(QString, directory);
        QString path = QDir::current().path() + QDir::toNativeSeparators(directory);

        QVERIFY2(!QDir(path).exists(), path.toLatin1());
        MkdirOperation op;
        op.setArguments(QStringList() << path);
        op.backup();
        QVERIFY2(op.performOperation(), op.errorString().toLatin1());
        QVERIFY2(QDir(path).exists(), path.toLatin1());
        QVERIFY2(op.undoOperation(), op.errorString().toLatin1());
        QVERIFY2(!QDir(path).exists(), path.toLatin1());
    }

    void testCreateDirectory_customFile_data()
    {
         QTest::addColumn<QString>("directory");
         QTest::addColumn<QString>("filename");
         QTest::newRow("/test") << "/test" << "/test/file.txt";
         QTest::newRow("/test/test") << "/test/test" << "/test/file.txt";
         QTest::newRow("/test/test/test") << "/test/test/test" << "/test/test/test/file.txt";
    }

    void testCreateDirectory_customFile()
    {
        QFETCH(QString, directory);
        QFETCH(QString, filename);
        QString path = QDir::current().path() + QDir::toNativeSeparators(directory);
        QString filepath = QDir::current().path() + QDir::toNativeSeparators(filename);

        QVERIFY2(!QDir(path).exists(), path.toLatin1());
        MkdirOperation op;
        op.setArguments(QStringList() << path);
        op.backup();
        QVERIFY2(op.performOperation(), op.errorString().toLatin1());
        QVERIFY2(QDir(path).exists(), path.toLatin1());
        QFile file(filepath);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << "This file is generated by QTest\n";
        file.close();
        QVERIFY2(!op.undoOperation(), op.errorString().toLatin1());
        QVERIFY2(file.exists(), filepath.toLatin1());
        QVERIFY2(QDir(filepath).remove(filepath), "Cannot remove file");
        QVERIFY2(!file.exists(), filepath.toLatin1());
        QVERIFY2(op.undoOperation(), op.errorString().toLatin1());
        QVERIFY2(!QDir(path).exists(), path.toLatin1());
    }

    void testCreateDirectory_customFile_force_data()
    {
        testCreateDirectory_customFile_data();
    }

    void testCreateDirectory_customFile_force()
    {
        QFETCH(QString, directory);
        QFETCH(QString, filename);
        QString path = QDir::current().path() + QDir::toNativeSeparators(directory);
        QString filepath = QDir::current().path() + QDir::toNativeSeparators(filename);

        QVERIFY2(!QDir(path).exists(), path.toLatin1());
        MkdirOperation op;
        op.setArguments(QStringList() << path);
        op.setValue("forceremoval",true);
        op.backup();
        QVERIFY2(op.performOperation(), op.errorString().toLatin1());
        QVERIFY2(QDir(path).exists(), path.toLatin1());
        QFile file(filepath);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << "This file is generated by QTest\n";
        file.close();
        QVERIFY2(op.undoOperation(), op.errorString().toLatin1());
        QVERIFY2(!file.exists(), path.toLatin1());
    }
};

QTEST_MAIN(tst_mkdiroperationtest)

#include "tst_mkdiroperationtest.moc"
