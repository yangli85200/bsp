# Driver-Demo Buildroot Integration Guide

## Overview

This document explains how driver-demo is integrated into the buildroot wrapper framework, demonstrating the key principle: **the driver source code is completely independent of buildroot and can be compiled standalone**.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│  Source Code (Buildroot-agnostic)                           │
│  source/2242v1r1/bsp/drivers/driver-demo/                   │
│  ├── driver-demo.c          # Driver implementation         │
│  ├── Makefile               # Standalone + BR compatible    │
│  └── README.md              # Documentation                 │
└─────────────────────────────────────────────────────────────┘
                              │
                              │ Referenced by
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  Buildroot Package Definition                               │
│  product-line-a4-monochrome/.../machine/bm5230/package/     │
│  └── bsp/driver-demo/                                       │
│      ├── Config.in          # Kconfig menu entry            │
│      └── driver-demo.mk     # Build instructions            │
└─────────────────────────────────────────────────────────────┘
```

## Key Design Principles

### 1. Source Code Independence

The driver source code in `source/2242v1r1/bsp/drivers/driver-demo/` knows **nothing** about buildroot:

- Uses standard Linux kernel Makefile conventions
- Can be compiled standalone with `make`
- No buildroot-specific variables or paths
- Portable to any build system

### 2. Buildroot Integration Layer

The package definition in `machine/bm5230/package/bsp/driver-demo/` provides the buildroot integration:

- `Config.in`: Adds driver to menuconfig
- `driver-demo.mk`: Tells buildroot how to build the driver
- Uses `OVERRIDE_SRCDIR` for development workflow

### 3. Incremental Compilation Support

Through `local.mk`, changes to driver source are immediately reflected:

```makefile
DRIVER_DEMO_OVERRIDE_SRCDIR = $(BR2_EXTERNAL_GENERAL_PATH)/../source/2242v1r1/bsp/drivers/driver-demo
```

## Usage Examples

### Standalone Compilation (No Buildroot)

```bash
cd source/2242v1r1/bsp/drivers/driver-demo

# Native build
make

# Cross-compilation
make ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- KDIR=/path/to/kernel

# Clean
make clean
```

### Buildroot Compilation

```bash
# From project root
make 2242v1r1_bm5230_defconfig

# Build driver through buildroot
cd output/2242v1r1_bm5230
make driver-demo

# Or from project root (slower)
make driver-demo
```

### Development Workflow

```bash
# 1. Modify driver source
vim source/2242v1r1/bsp/drivers/driver-demo/driver-demo.c

# 2. Quick test - standalone build
cd source/2242v1r1/bsp/drivers/driver-demo
make clean && make

# 3. Full integration test - buildroot rebuild
cd ../../../../../output/2242v1r1_bm5230
make driver-demo-rebuild

# 4. Deploy to target
# The .ko file is automatically installed to target rootfs
```

## How It Works

### 1. Package Definition (driver-demo.mk)

```makefile
DRIVER_DEMO_VERSION = 1.0
DRIVER_DEMO_SITE = $(BR2_EXTERNAL_GENERAL_PATH)/../source/2242v1r1/bsp/drivers/driver-demo
DRIVER_DEMO_SITE_METHOD = local  # Use local source
DRIVER_DEMO_DEPENDENCIES = linux  # Depends on kernel

# Build using kernel build system
define DRIVER_DEMO_BUILD_CMDS
	$(LINUX_MAKE_ENV) $(MAKE) -C $(LINUX_DIR) \
		M=$(@D) \
		modules
endef
```

### 2. Kernel Module Infrastructure

Buildroot's `$(eval $(kernel-module))` provides:
- Automatic dependency on Linux kernel
- Proper kernel build environment
- Module installation to `/lib/modules/`
- Automatic depmod execution

### 3. Source Override

The `OVERRIDE_SRCDIR` mechanism:
- Points buildroot to local source directory
- Skips download/extract steps
- Enables incremental compilation
- Perfect for active development

## File Locations

| Component | Path | Purpose |
|-----------|------|---------|
| Driver Source | `source/2242v1r1/bsp/drivers/driver-demo/` | Buildroot-agnostic code |
| Package Config | `machine/bm5230/package/bsp/driver-demo/Config.in` | Menuconfig entry |
| Package Build | `machine/bm5230/package/bsp/driver-demo/driver-demo.mk` | Build rules |
| BSP Menu | `machine/bm5230/package/bsp/Config.in` | Include driver in menu |
| Local Override | `machine/bm5230/local.mk` | Development mode |
| Defconfig | `configs/2242v1r1_bm5230_defconfig` | Enable by default |

## Adding a New Driver

To add another driver following this pattern:

1. **Create source code** (buildroot-independent):
   ```bash
   mkdir -p source/2242v1r1/bsp/drivers/my-driver
   # Add .c files and standalone Makefile
   ```

2. **Create buildroot package**:
   ```bash
   mkdir -p machine/bm5230/package/bsp/my-driver
   # Add Config.in and my-driver.mk
   ```

3. **Update BSP Config.in**:
   ```kconfig
   source "$BR2_EXTERNAL_GENERAL_PATH/../.../my-driver/Config.in"
   ```

4. **Add to local.mk** (optional, for development):
   ```makefile
   MY_DRIVER_OVERRIDE_SRCDIR = $(BR2_EXTERNAL_GENERAL_PATH)/../source/.../my-driver
   ```

5. **Enable in defconfig** (optional):
   ```
   BR2_PACKAGE_MY_DRIVER=y
   ```

## Benefits of This Approach

✅ **Portability**: Driver can be used outside buildroot
✅ **Development Speed**: Quick iteration with standalone builds
✅ **Integration**: Full buildroot integration when needed
✅ **Maintainability**: Clear separation of concerns
✅ **Testing**: Easy to test in multiple environments
✅ **Reusability**: Same source for different projects

## Troubleshooting

### Driver won't build in buildroot

```bash
# Check if kernel is built
make linux

# Check package is enabled
make menuconfig
# Navigate to: BSP Packages -> driver-demo

# Verbose build
make driver-demo V=1
```

### Changes not reflected

```bash
# Force rebuild
make driver-demo-dirclean
make driver-demo

# Or use rebuild
make driver-demo-rebuild
```

### Module not loading on target

```bash
# Check kernel version match
modinfo /lib/modules/*/extra/driver-demo.ko

# Check dependencies
lsmod
depmod -a
modprobe driver-demo
```

## References

- [Buildroot Manual - Kernel Modules](https://buildroot.org/downloads/manual/manual.html#_infrastructure_for_packages_building_kernel_modules)
- [Linux Kernel Module Programming Guide](https://sysprog21.github.io/lkmpg/)
- Driver source: `source/2242v1r1/bsp/drivers/driver-demo/README.md`

