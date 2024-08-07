/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2022 Synergy Ltd.
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QTimer>

class QNetworkReply;
class AppConfig;

class LicenseRegistry : public QObject {
  Q_OBJECT

public:
  explicit LicenseRegistry(AppConfig &config);
  void scheduleRegistration();

public slots:
  void registerLicense();
  void handleResponse(QNetworkReply *reply);

private:
  AppConfig &m_config;
  QNetworkAccessManager m_manager;
  QTimer m_timer;

  QByteArray getRequestData() const;
};
