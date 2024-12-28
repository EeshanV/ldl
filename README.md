# ldl

## Overview
This project is a collection of Linux kernel modules. Each module is designed to help deepen understanding of Linux kernel concepts.

## Project Structure
Each module resides in its own folder, containing the source code, Makefile, and any additional resources required for building and testing.

### Completed Modules
| Module Folder                 | Description                                                                                     |
|-------------------------------|-------------------------------------------------------------------------------------------------|
| `hello-world`                 | A simple module that logs "Hello, world!" to the kernel log, serving as an introduction to module creation. |
| `dev-number`                  | Module demonstrating how to obtain a device number and interact with device files.              |
| `read-write`                  | Module that automatically creates device files for easy access and manipulation of device data. |
| `semaphore`                   | Semaphore-based character driver that synchronizes concurrent access to a shared buffer         |
| `spinlock`                    | Spinlock-based character driver that protects a shared buffer from concurrent access.           |
| `zeros`                       | Module replicating the functionality of `/dev/zero` but return '0' char instead of null character |
| `memory-allocator`            | Module to demonstrate basic memory allocation and deallocation using `kmalloc` and `vmalloc`.    |

*Additional modules will be added over time as the project progresses.*
