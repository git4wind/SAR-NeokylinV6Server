#Summary 软件包简要介绍
Summary:hba host audit for linux

#软件包名称。默认hbla
Name:hbla

#软件包主版本号，需要和source目录中的目录名称对应,默认1.0.1
Version:1.0.1

#软件包此版本号，用于表示客户或者特定 1.跨平台
Release:1

#授权协议
License:CopyRight for hba.cn

#软件分类
Group:Applications/System
URL:http://hba.cn
Distribution:HBA host audit for Linux
Packager:pc<xupengcheng@hba.cn>

Source0:%{name}-%{version}.tar.gz
#TempBuildPath
BuildRoot:%{_tmppath}/%{name}-%{version}-%{release}-root
%description
#预处理操作字段，在build前执行,解压安装包
%prep
%setup -q

#build用于建立目录，源码中用于./configure make make noting
%build
#rm -rf $RPM_BUILD_ROOT
#mkdir $RPM_BUILD_ROOT
#mkdir $RPM_BUILD_ROOT/usr/
#mkdir $RPM_BUILD_ROOT/usr/local
#mkdir $RPM_BUILD_ROOT/usr/local/hb_t
#mkdir $RPM_BUILD_ROOT/usr/local/hb_t/libs
#mkdir $RPM_BUILD_ROOT/usr/local/hb_t/lib
#mkdir $RPM_BUILD_ROOT/usr/local/hb_t/conf
#mkdir $RPM_BUILD_ROOT/usr/local/hb_t/tools
#mkdir $RPM_BUILD_ROOT/usr/local/hb_t/tools/imgs
#install字段用于安装,noting,把打包文件拷贝到虚拟路径
%install
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT/usr/
mkdir $RPM_BUILD_ROOT/usr/local
mkdir $RPM_BUILD_ROOT/usr/local/hb_t
mkdir $RPM_BUILD_ROOT/usr/local/hb_t/libs
mkdir $RPM_BUILD_ROOT/usr/local/hb_t/lib
mkdir $RPM_BUILD_ROOT/usr/local/hb_t/conf
mkdir $RPM_BUILD_ROOT/usr/local/hb_t/tools
mkdir $RPM_BUILD_ROOT/usr/local/hb_t/tools/imgs
cp hbla $RPM_BUILD_ROOT/usr/local/hb_t
cp libs/* $RPM_BUILD_ROOT/usr/local/hb_t/libs/
cp lib/* $RPM_BUILD_ROOT/usr/local/hb_t/lib/
cp conf/* $RPM_BUILD_ROOT/usr/local/hb_t/conf/
cp -rf tools/* $RPM_BUILD_ROOT/usr/local/hb_t/tools/
#用于指定需要打包的文件
%files
/usr/local/hb_t
%pre
if [ -f "/usr/local/hb/.installed" ];then
	exit
fi
%post
if [ -f "/usr/local/hb/.installed" ];then
	exit
fi
cp -rf  %{_prefix}/local/hb_t %{_prefix}/local/hb
cp -f %{_prefix}/local/hb/lib/libhbla_hostinfo.so %{_prefix}/lib
cp -f %{_prefix}/local/hb/lib/libhbla_sslcon.so %{_prefix}/lib
cp -f %{_prefix}/local/hb/lib/libpcap.so.1 %{_prefix}/local/lib
cp -f %{_prefix}/local/hb/tools/hbla %{_sysconfdir}/init.d
ln -sf /etc/init.d/hbla %{_sysconfdir}/rc5.d/S100hbla
cp -f %{_prefix}/local/hb/tools/hbmanager %{_prefix}/bin
cp -f %{_prefix}/local/hb/tools/hbreport %{_prefix}/bin
ldconfig
touch %{_prefix}/local/hb/.installed
service hbla start
%preun
exit
%postun
exit
