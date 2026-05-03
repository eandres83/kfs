# KFS-6 - Filesystem & Persistent Storage

![Arch](https://img.shields.io/badge/arch-x86-lightgrey)
![Storage](https://img.shields.io/badge/storage-IDE%20%2F%20PATA-red)
![Filesystem](https://img.shields.io/badge/filesystem-VFS%20%2F%20Ext2-blue)

<br />
<p align="center">
  <h3 align="center">Phase 6: Disk, Files and Format</h3>
</p>

## 🗣️ About The Project

**KFS-6** introduces non-volatile data storage to the kernel. Before this phase, the OS existed entirely in RAM and lost all state upon shutdown. Now, the kernel can interact with physical disk drives using the **IDE (Parallel ATA)** interface.

This milestone required building a **Virtual File System (VFS)** to abstract filesystem operations, and implementing the **Ext2** filesystem protocol to parse inodes, superblocks, and directories.

### 🎯 Key Engineering Achievements
- **IDE Controller Driver:** Developed a low-level device driver reading/writing 512-byte blocks from the IDE mass storage controller via isolated I/O ports (`0x1F0`).
- **Virtual File System (VFS):** Implemented an abstraction layer defining a standardized filesystem node structure containing Name, Size, Type, Inode, Links, Father, Children, and Rights.
- **Ext2 Implementation:** Engineered a parser for the *Second Extended Filesystem*. The kernel correctly locates the Superblock, reads Block Group Descriptors, and traverses the Inode tables to reconstruct the file tree in memory.
- **Process Isolation:** The current working directory (`pwd`) is now tied to the process's Task Control Block (TCB), allowing concurrent processes to navigate different directories simultaneously.
- **Bonuses Achieved:**
  - **MBR Parsing:** Added support for the Master Boot Record to locate, identify, and handle multiple partitions on a single disk.
  - **Mounting System:** Integrated `mount` and `umount` capabilities to dynamically attach block devices to the VFS tree.
  - **User Management:** Implemented login protocols and user authentication (passwords) tied to file access rights.

---

## 🏗️ Storage Architecture

The kernel uses a layered approach to translate high-level user commands into low-level electrical signals sent to the hard drive.

~~~mermaid
graph TD
    subgraph User Space
        CLI["Shell Commands (cat, cd, pwd)"]
    end
    
    subgraph Kernel Space
        VFS["Virtual File System (VFS)"]
        EXT2["Ext2 Driver"]
        MBR["Partition Manager (MBR)"]
    end
    
    subgraph Hardware Layer
        IDE["IDE / PATA Controller"]
        DISK[("Physical Hard Drive")]
    end

    CLI -->|Path Translation| VFS
    VFS -->|Inode Request| EXT2
    EXT2 -->|LBA Block Request| MBR
    MBR -->|Port I/O| IDE
    IDE <--> DISK
~~~

---

## 📂 Repository Structure (KFS-6 additions)

~~~text
.
├── src/
│   ├── drivers/
│   │   └── ide/
│   │       ├── ide.c        # IDE Controller initialization and I/O logic
│   │       └── ide.h        # Hardware port definitions
│   ├── fs/
│   │   ├── ext2/            # Ext2 parser (Superblocks, Inodes, Directories)
│   │   ├── vfs/             # Virtual File System abstraction layer
│   │   ├── mbr.c            # Master Boot Record parsing
│   │   └── dummy.c          # Mock filesystem structures for testing
│   └── kernel/
│       └── command.c        # File manipulation commands (cat, cd, pwd)
└── ...
~~~

---

## 🚀 Usage & Commands

Compile and run the kernel in QEMU:

~~~bash
make run
~~~

### Filesystem Shell Commands
Once booted, use the following commands to navigate the Ext2 filesystem:

| Command | Description |
| :--- | :--- |
| **`pwd`** | Prints the current working directory of the active process. |
| **`cd <path>`** | Changes the current working directory. |
| **`cat <file>`** | Reads an Ext2 inode, extracts the data blocks, and prints the file content to standard output. |

---
*Developed by Eleder Andres. KFS-6 Release.*
