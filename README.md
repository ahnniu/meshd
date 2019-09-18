# meshctld

A Bluetooth mesh gateway daemon based on meshctl of bluez@5.50

## Setup on Raspberry Pi 3

### Install Dependencies

#### Install Dependencies for BlueZ

```bash
$ sudo apt-get install autoconf libtool libtool-bin libglib2.0-dev libdbus-1-dev libjson-c-dev elfutils libelf-dev libdw-dev udev libudev-dev systemd libsystemd-dev libasound2-dev libical-dev libreadline-dev libsbc-dev libspeexdsp-dev automake qt3d5-dev libncurses5-dev pesign
```

#### Install json-c

```bash
$ cd ~
$ wget https://s3.amazonaws.com/json-c_releases/releases/json-c-0.13.tar.gz
$ tar -xvf json-c-0.13.tar.gz
$ cd json-c-0.13/
$ ./configure --prefix=/usr --disable-static && make
$ sudo make install
```
#### Install ell for BlueZ v5.50

```bash
$ cd ~
$ wget https://mirrors.edge.kernel.org/pub/linux/libs/ell/ell-0.6.tar.xz
$ tar -xvf ell-0.6.tar.xz
$ cd ell-0.6/
$ ./configure --prefix=/usr
$ make
$ sudo make install
```
### Get source code

```bash
$ cd ~
$ git clone https://github.com/ahnniu/meshd.git
$ cd meshd
$ git checkout meshd
```
### Compile

```bash
$ ./bootstrap
$ ./configure --enable-mesh --prefix=/usr --mandir=/usr/share/man --sysconfdir=/etc --localstatedir=/var
$ make
```

### Install

TBD

## How to use

### Start the meshd

```bash
$ cd /path/to/meshd
# Start in shell interactive mode
$ sudo mesh/meshctl --config mesh
# Start in daemon mode
$ sudo mesh/meshctl --config mesh --daemon 1 --log mesh/log.txt
```

### DBus configuration

```bash
$ cd /path/to/meshd
$ sudo cp mesh/conf/embest.conf /etc/dbus-1/system.d/
$ sudo reboot
```

### DBus interface

- [DBus Strategy](mesh/doc/dbus-strategy.md)
- [DBus Provisioner](mesh/doc/dbus-prov.md)
- [DBus Config](mesh/doc/dbus-config.md)

