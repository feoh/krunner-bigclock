Name:           krunner-bigclock
Version:        0.1.0
Release:        1%{?dist}
Summary:        KRunner plugin that displays a large configurable clock

License:        MIT
URL:            https://github.com/feoh/krunner-bigclock
Source0:        %{url}/archive/refs/tags/v%{version}/%{name}-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  extra-cmake-modules
BuildRequires:  gcc-c++
BuildRequires:  kf6-kcmutils-devel
BuildRequires:  kf6-kconfig-devel
BuildRequires:  kf6-ki18n-devel
BuildRequires:  kf6-krunner-devel
BuildRequires:  qt6-qtbase-devel

Requires:       kf6-kcmutils
Requires:       kf6-kconfig
Requires:       kf6-ki18n
Requires:       kf6-krunner
Requires:       qt6-qtbase

%description
KRunner Bigclock adds a KRunner result that launches a large, centered clock.
The clock can be displayed as an LED, Nixie tube, or mechanical style clock.

%prep
%autosetup

%build
%cmake -DBUILD_TESTING=OFF
%cmake_build

%install
%cmake_install

%files
%license LICENSES/MIT.txt
%doc README.md
%{_bindir}/krunner-bigclock-window
%{_libdir}/qt6/plugins/kf6/krunner/krunner_bigclock.so
%{_libdir}/qt6/plugins/kf6/krunner/kcms/kcm_krunner_bigclock.so
%{_metainfodir}/org.kde.krunner-bigclock.metainfo.xml

%changelog
* Wed May 13 2026 Chris Patti <feoh@feoh.org> - 0.1.0-1
- Initial package
