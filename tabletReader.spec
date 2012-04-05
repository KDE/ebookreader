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

Name:           tabletReader
Version:        2.0.0
Release:        1
License:        GPLv2+
Summary:        Document Viewer
Url:            http://www.kde.org
Group:          Productivity/Office/Other
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  qt-mobility-devel >= 1.2
BuildRequires:  OpenEXR-devel
BuildRequires:  chmlib-devel
BuildRequires:  libdjvulibre-devel
BuildRequires:  libkde4-devel
BuildRequires:  libpoppler-qt4-devel
BuildRequires:  oxygen-icon-theme-large
BuildRequires:  soprano-backend-redland
%if 0%{?suse_version} > 1130
BuildRequires:  sane-backends-devel
%else
BuildRequires:  sane-backends
%endif
Obsoletes:      kdegraphics4 < 4.7.0
Provides:       kdegraphics4 = 4.7.0
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
%requires_ge    libpoppler-qt4-3
%kde4_runtime_requires

%description
E-book reader for touch-enabled devices; supports documents in PDF, DJVU and
CHM.

%package devel
License:        GPLv2+
Summary:        E-Book Reader - Development Files
Group:          Development/Libraries/KDE
Requires:       libkde4-devel
Requires:       %{name} = %{version}
Provides:       kde4-%{name}-devel = 4.3.0
Obsoletes:      kde4-%{name}-devel < 4.3.0
%kde4_runtime_requires

%description devel
E-book reader for touch enabled-devices; supports document in various formats

%prep
%setup -q

%build
  qmake
  make

%install
  make install INSTALL_ROOT=%{buildroot}
  %suse_update_desktop_file -r tabletReader   Office Viewer
  %fdupes -s %{buildroot}%{_datadir}
  %kde_post_install

%clean
  make clean

%files
%defattr(-,root,root)
%{_kde4_applicationsdir}/tabletReader.desktop
%{_kde4_appsdir}/tabletReader/
%{_kde4_bindir}/tabletReader
%config %{_kde4_configkcfgdir}/*.kcfg
%{_kde4_htmldir}/en/tabletReader/
%{_kde4_iconsdir}/hicolor/*/apps/graphics-viewer-document.*
%{_kde4_iconsdir}/hicolor/*/apps/tabletReader.*
%{_kde4_modulesdir}/*.so
%{_kde4_servicesdir}/*.desktop
%{_kde4_servicesdir}/*.protocol
%{_kde4_servicetypesdir}/*.desktop

%files devel
%defattr(-,root,root)
%{_kde4_includedir}/tabletReader/

%changelog
