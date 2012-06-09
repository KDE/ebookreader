#
# spec file for package atReader
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

Name:           atReader
Version:        2.0.0
Release:        1
License:        GPLv2+
Summary:        Document Viewer
Url:            http://www.kde.org
Group:          Productivity/Office/Other
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  qt-mobility-devel >= 1.2
BuildRequires:  chmlib-devel
BuildRequires:  libdjvulibre-devel
BuildRequires:  libkde4-devel
BuildRequires:  libpoppler-qt4-devel
Obsoletes:      kdegraphics4 < 4.7.0
Provides:       kdegraphics4 = 4.7.0
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
%requires_ge    libpoppler-qt4-3
%kde4_runtime_requires

%description
E-book reader for touch-enabled devices; supports documents in PDF, DJVU and
CHM.

%prep
%setup -q

%build
  qmake
  make

%install
  make install INSTALL_ROOT=%{buildroot}
  mkdir -p %{buildroot}%{_kde4_applicationsdir}
  cp atReader.desktop %{buildroot}%{_kde4_applicationsdir}
  mkdir -p %{buildroot}%{_kde4_appsdir}/atReader/
  mkdir -p %{buildroot}%{_kde4_bindir}
  mv %{buildroot}/atReader %{buildroot}%{_kde4_bindir}
  mkdir -p %{buildroot}%{_kde4_iconsdir}/hicolor/{16x16,22x22,32x32,48x48,64x64,128x128}/apps
  cp src/icons/128x128/atReader.png %{buildroot}%{_kde4_iconsdir}/hicolor/128x128/apps/
  cp src/icons/64x64/atReader.png %{buildroot}%{_kde4_iconsdir}/hicolor/64x64/apps/
  cp src/icons/48x48/atReader.png %{buildroot}%{_kde4_iconsdir}/hicolor/48x48/apps/
  cp src/icons/32x32/atReader.png %{buildroot}%{_kde4_iconsdir}/hicolor/32x32/apps/
  cp src/icons/22x22/atReader.png %{buildroot}%{_kde4_iconsdir}/hicolor/22x22/apps/
  cp src/icons/16x16/atReader.png %{buildroot}%{_kde4_iconsdir}/hicolor/16x16/apps/
  %suse_update_desktop_file -r atReader   Office Viewer
  %kde_post_install

%clean
  rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_kde4_applicationsdir}/atReader*.desktop
%{_kde4_appsdir}/atReader/
%{_kde4_bindir}/atReader
%{_kde4_iconsdir}/hicolor/*/apps/atReader.*

%changelog
