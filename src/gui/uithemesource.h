/*
 * Bittorrent Client using Qt and libtorrent.
 * Copyright (C) 2023  Vladimir Golovnev <glassez@yandex.ru>
 * Copyright (C) 2019  Prince Gupta <jagannatharjun11@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * In addition, as a special exception, the copyright holders give permission to
 * link this program with the OpenSSL project's "OpenSSL" library (or with
 * modified versions of it that use the same license as the "OpenSSL" library),
 * and distribute the linked executables. You must obey the GNU General Public
 * License in all respects for all of the code used other than "OpenSSL".  If
 * you modify file(s), you may extend this exception to your version of the
 * file(s), but you are not obligated to do so. If you do not wish to do so,
 * delete this exception statement from your version.
 */

#pragma once

#include <memory>

#include <QtGlobal>
#include <QColor>
#include <QHash>
#include <QIcon>
#include <QString>

#include "base/path.h"
#include "uithemecommon.h"

enum class ColorMode
{
    Light,
    Dark
};

class UIThemeSource
{
    Q_DECLARE_TR_FUNCTIONS(UIThemeSource)

public:
    virtual ~UIThemeSource() = default;

    virtual QColor getColor(const QString &colorId, const ColorMode colorMode) const = 0;
    virtual Path getIconPath(const QString &iconId, const ColorMode colorMode) const = 0;
    virtual QByteArray readStyleSheet() = 0;
};

class DefaultThemeSource final : public UIThemeSource
{
public:
    DefaultThemeSource();

    QByteArray readStyleSheet() override;
    QColor getColor(const QString &colorId, const ColorMode colorMode) const override;
    Path getIconPath(const QString &iconId, const ColorMode colorMode) const override;

private:
    void loadColors();

    const Path m_defaultPath;
    const Path m_userPath;
    QHash<QString, UIThemeColor> m_colors;
};

class CustomThemeSource : public UIThemeSource
{
public:
    QColor getColor(const QString &colorId, const ColorMode colorMode) const override;
    Path getIconPath(const QString &iconId, const ColorMode colorMode) const override;
    QByteArray readStyleSheet() override;

protected:
    virtual Path themeRootPath() const = 0;
    DefaultThemeSource *defaultThemeSource() const;

private:
    void loadColors();

    const std::unique_ptr<DefaultThemeSource> m_defaultThemeSource = std::make_unique<DefaultThemeSource>();
    QHash<QString, QColor> m_colors;
    QHash<QString, QColor> m_darkModeColors;
};

class QRCThemeSource final : public CustomThemeSource
{
private:
    Path themeRootPath() const override;
};

class FolderThemeSource : public CustomThemeSource
{
public:
    explicit FolderThemeSource(const Path &folderPath);

    QByteArray readStyleSheet() override;

private:
    Path themeRootPath() const override;

    const Path m_folder;
};
