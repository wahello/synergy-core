/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2015 Synergy Ltd.
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

#include "ActivationNotifier.h"

#include "CoreInterface.h"

ActivationNotifier::ActivationNotifier(QObject *parent) : QObject(parent) {}

void ActivationNotifier::setIdentity(QString identity) {
  m_Identity = identity;
}

void ActivationNotifier::setUpdateInfo(QString const &fromVersion,
                                       QString const &toVersion,
                                       QString const &serialKey) {
  m_fromVersion = fromVersion;
  m_toVersion = toVersion;
  m_serialKey = serialKey;
}

void ActivationNotifier::notify() {
  // TODO: use something other than CURL
}

void ActivationNotifier::notifyUpdate() {
  // TODO: use something other than CURL
}
