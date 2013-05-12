#
# spec file for package tabletReader
#
# Copyright (c) 2011 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Name:           TabletReader
Version:        3.0.0
Release:        1
License:        GPLv2+
Summary:        Document Viewer
Url:            https://projects.kde.org/projects/playground/edu/ebookreader
Group:          Productivity/Office/Other
Source0:        %{name}-%{version}.tar.gz
Requires:       %{kde_runtime_requires}
BuildRequires:  pkgconfig(exiv2)
BuildRequires:  pkgconfig(shared-mime-info)
BuildRequires:  pkgconfig(lcms)
BuildRequires:  pkgconfig(poppler-qt4)
BuildRequires:  pkgconfig(qca2)
BuildRequires:  pkgconfig(xext)
BuildRequires:  pkgconfig(xdamage)
BuildRequires:  pkgconfig(xrandr)
BuildRequires:  pkgconfig(xrender)
BuildRequires:  pkgconfig(xcursor)
BuildRequires:  pkgconfig(sm)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(zlib)
BuildRequires:  pkgconfig(freetype2)
BuildRequires:  pkgconfig(soprano)
BuildRequires:  pkgconfig(qimageblitz)
BuildRequires:  pkgconfig(shared-desktop-ontologies)
BuildRequires:  pkgconfig(libkactivities)
BuildRequires:  plasma-mobile-devel
BuildRequires:  kdelibs-devel
BuildRequires:  oxygen-icon-theme-large
BuildRequires:  libjpeg-devel
BuildRequires:  strigiclient-devel
BuildRequires:  strigidaemon-devel
BuildRequires:  libtiff-devel
BuildRequires:  libepub-devel
BuildRequires:  okular-devel
BuildRequires:  cmake

%description
E-book reader for touch-enabled devices; uses as backend okular core library.

%prep
%setup -q -n %{name}-%{version}

%build
  cmake . %{cmake_kde_options}
  make %{?jobs:-j%jobs}

%install
  %make_install

%clean
  rm -rf %{buildroot}

%files
%defattr(-,root,root)
/usr/share/applications/kde4//tabletReader*.desktop
/usr/share/doc/tabletReaderHelp.pdf
/usr/bin/tabletReader/
/usr/share/icons/hicolor/*/apps/tabletReader.*  

%changelog
