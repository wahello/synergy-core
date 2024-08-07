/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2020-2020 Symless Ltd.
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

#include <cassert>

#include <QCoreApplication>
#include <QFile>

#include "ConfigBase.h"
#include "ConfigWriter.h"

namespace {

QString getSystemSettingPath() {
  const QString settingFilename("SystemConfig.ini");
  QString path;
#if defined(Q_OS_WIN)
  // Program file
  path = QCoreApplication::applicationDirPath() + "\\";
#elif defined(Q_OS_DARWIN)
  // Global preferances dir
  //  Would be nice to use /library, but QT has no elevate system in place
  path = "/usr/local/etc/symless/";
#elif defined(Q_OS_LINUX)
  // QT adds application and filename to the end of the path already on linux
  path = "/usr/local/etc/symless/";
  return path;
#else
  assert("OS not supported");
#endif
  return path + settingFilename;
}

#if defined(Q_OS_WIN)
void loadOldSystemSettings(QSettings &settings) {
  if (!QFile(settings.fileName()).exists()) {
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope,
                       "SystemConfig.ini");
    QSettings oldSystemSettings(QSettings::IniFormat, QSettings::SystemScope,
                                QCoreApplication::organizationName(),
                                QCoreApplication::applicationName());

    if (QFile(oldSystemSettings.fileName()).exists()) {
      for (const auto &key : oldSystemSettings.allKeys()) {
        settings.setValue(key, oldSystemSettings.value(key));
      }
    }

    // Restore system settings path
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope,
                       getSystemSettingPath());
  }
}
#endif

} // namespace

namespace GUI {
namespace Config {
// Assignment of static variable
ConfigWriter *ConfigWriter::s_pConfiguration = nullptr;

ConfigWriter *ConfigWriter::make() {
  // Only one ConfigWriter can exist at any one time (Singleton)
  if (!s_pConfiguration) {
    s_pConfiguration = new ConfigWriter();
  }
  return s_pConfiguration;
}

ConfigWriter::ConfigWriter() {
  QSettings::setPath(QSettings::Format::IniFormat,
                     QSettings::Scope::SystemScope, getSystemSettingPath());

  // Config will default to User settings if they exist,
  //  otherwise it will load System setting and save them to User settings
  m_pSettingsSystem =
      new QSettings(QSettings::Format::IniFormat, QSettings::Scope::SystemScope,
                    QCoreApplication::organizationName(),
                    QCoreApplication::applicationName());

#if defined(Q_OS_WIN)
  // This call is needed for backwardcapability with old settings.
  loadOldSystemSettings(*m_pSettingsSystem);
#endif

  // defaults to user scope, if we set the scope specifically then we also have
  // to set
  //  the application name and the organisation name which breaks backwards
  //  compatibility See #6730
  m_pSettingsUser = new QSettings();

  // Set scope to user for initially
  m_pSettingsCurrent = m_pSettingsUser;
}

void ConfigWriter::destroy() { destroy(s_pConfiguration); }

ConfigWriter::~ConfigWriter() {
  while (!m_pCallerList.empty()) {
    m_pCallerList.pop_back();
  }
  m_pSettingsCurrent = nullptr; // this only references other pointers
  destroy(m_pSettingsSystem);
  destroy(m_pSettingsUser);
}

bool ConfigWriter::hasSetting(const QString &name, Scope scope) const {
  switch (scope) {
  case kUser:
    return m_pSettingsUser->contains(name);
  case kSystem:
    return m_pSettingsSystem->contains(name);
  default:
    return m_pSettingsCurrent->contains(name);
  }
}

bool ConfigWriter::isWritable() const {
  return m_pSettingsCurrent->isWritable();
}

QVariant ConfigWriter::loadSetting(const QString &name,
                                   const QVariant &defaultValue, Scope scope) {
  switch (scope) {
  case kUser:
    return m_pSettingsUser->value(name, defaultValue);
  case kSystem:
    return m_pSettingsSystem->value(name, defaultValue);
  default:
    return m_pSettingsCurrent->value(name, defaultValue);
  }
}

void ConfigWriter::setScope(ConfigWriter::Scope scope) {
  if (m_CurrentScope != scope) {
    m_CurrentScope = scope;
    switch (scope) {
    case kUser:
      m_pSettingsCurrent = m_pSettingsUser;
      break;
    case kSystem:
      m_pSettingsCurrent = m_pSettingsSystem;
      break;
    default:
      // setScope should never be kCurrent
      assert(scope);
    }
  }
}

ConfigWriter::Scope ConfigWriter::getScope() const { return m_CurrentScope; }

void ConfigWriter::globalLoad() {
  for (auto &i : m_pCallerList) {
    i->loadSettings();
  }
}

void ConfigWriter::globalSave() {

  // Save if there are any unsaved changes otherwise skip
  if (unsavedChanges()) {
    for (auto &i : m_pCallerList) {
      i->saveSettings();
    }

    m_pSettingsUser->sync();
    m_pSettingsSystem->sync();

    m_unsavedChanges = false;
  }
}

QSettings &ConfigWriter::settings() { return *m_pSettingsCurrent; }

void ConfigWriter::registerClass(ConfigBase *receiver) {
  m_pCallerList.push_back(receiver);
}

bool ConfigWriter::unsavedChanges() const {
  if (m_unsavedChanges) {
    return true;
  }

  for (const auto &i : m_pCallerList) {
    if (i->modified()) {
      // If any class returns true there is no point checking more
      return true;
    }
  }
  // If this line is reached no class has unsaved changes
  return false;
}

void ConfigWriter::markUnsaved() { m_unsavedChanges = true; }
} // namespace Config
} // namespace GUI
