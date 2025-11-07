# Driver Demo - Pantum Printer Driver Example

A simple Linux kernel module demonstrating driver development for Pantum printers.

## Features

- Character device driver
- Read/Write operations
- Sysfs integration
- Standalone compilation support
- Buildroot integration

## Standalone Compilation

### Prerequisites

```bash
# Install kernel headers
sudo apt-get install linux-headers-$(uname -r)

# For cross-compilation
sudo apt-get install gcc-riscv64-linux-gnu
```

### Build

```bash
cd source/2242v1r1/bsp/drivers/driver-demo

# Native build
make

# Cross-compilation for RISC-V
make ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- KDIR=/path/to/kernel/build
```

### Install and Test

```bash
# Load the module
sudo insmod driver-demo.ko

# Check if loaded
lsmod | grep driver_demo
dmesg | tail

# Test the device
cat /dev/driver_demo
echo "test" > /dev/driver_demo

# Unload the module
sudo rmmod driver_demo
```

## Buildroot Integration

This driver is automatically integrated into buildroot through the package system.

### Enable in menuconfig

```bash
make menuconfig
# Navigate to: Project Private Packages (Extend) -> BSP Packages -> driver-demo
```

### Build with buildroot

```bash
# Build only the driver
make driver-demo

# Rebuild after source changes
make driver-demo-rebuild

# Clean and rebuild
make driver-demo-dirclean
make driver-demo
```

## Directory Structure

```
driver-demo/
├── driver-demo.c    # Driver source code
├── Makefile         # Build system (standalone + buildroot)
└── README.md        # This file
```

## Development Workflow

1. **Modify source code**: Edit `driver-demo.c`
2. **Test standalone**: `make clean && make` (quick iteration)
3. **Test in buildroot**: `make driver-demo-rebuild` (full integration test)

## Module Information

- **Device Name**: `/dev/driver_demo`
- **Major Number**: Dynamically allocated
- **Class Name**: `driver_demo_class`

## Kernel Messages

```bash
# View driver messages
dmesg | grep driver-demo

# Monitor in real-time
dmesg -w | grep driver-demo
```

## Troubleshooting

### Module won't load

```bash
# Check kernel version compatibility
modinfo driver-demo.ko

# Check dependencies
modprobe --show-depends driver-demo
```

### Permission denied

```bash
# Check device permissions
ls -l /dev/driver_demo

# Fix permissions if needed
sudo chmod 666 /dev/driver_demo
```

## License

GPL v2

