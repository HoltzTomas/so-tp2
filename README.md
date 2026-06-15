# TP2 — Sistemas Operativos (72.11)

Kernel de sistema operativo de 64 bits sobre x86-64 bare metal. Implementa gestión de memoria, procesos con context switch, scheduler con prioridades, semáforos, pipes, keyboard driver bloqueante y una shell interactiva con 20 comandos.

## Requisitos

- **Docker** (imagen `agodio/itba-so-multiarch:3.1`)
- **QEMU** (`qemu-system-x86_64`)

## Compilar

```bash
# Compilar con First Fit memory manager (default)
docker run --rm -v "$(pwd)":/src agodio/itba-so-multiarch:3.1 make all

# Compilar con Buddy System memory manager
docker run --rm -v "$(pwd)":/src agodio/itba-so-multiarch:3.1 make buddy

# Limpiar
docker run --rm -v "$(pwd)":/src agodio/itba-so-multiarch:3.1 make clean
```

## Ejecutar

```bash
# Ejecutar en QEMU (detecta OS y audio automáticamente)
./run.sh

# O manualmente
qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
```

## Estructura del proyecto

```
├── Bootloader/           # Bootloader Pure64 + BMFS
├── Kernel/
│   ├── asm/              # Interrupts, context switch, libasm
│   ├── drivers/          # Video driver, keyboard driver
│   ├── idt/              # IDT, syscalls, exceptions, timer
│   ├── include/          # Headers del kernel
│   ├── lib/              # Utilidades (lib.c, list.c)
│   ├── memory/           # First Fit y Buddy System
│   ├── pipes/            # Pipes unidireccionales
│   ├── processes/        # PCB, scheduler, idle process
│   └── semaphores/       # Semáforos con XCHG atómico
├── Userland/
│   ├── libc/             # Biblioteca estándar (stdio, stdlib)
│   ├── shell/            # Shell interactiva
│   │   ├── commands/     # Comandos modulares (.c por comando)
│   │   └── shell.c       # Parser, pipe |, background &
│   └── tests/            # Tests de la cátedra
├── Makefile              # Build principal
└── run.sh                # Script de ejecución QEMU
```

## Memory Managers

El kernel soporta dos memory managers intercambiables por flag de compilación:

| Manager | Flag | Estrategia | Overhead |
|---------|------|-----------|----------|
| **First Fit** | `-DFIRSTFIT` (default) | Lista circular con coalescing bidireccional | 16 bytes/bloque |
| **Buddy System** | `-DBUDDY` | Páginas de 4KB, órdenes 0-10, merge recursivo | Bitmap por orden |

## Comandos de la Shell

### Generales
| Comando | Descripción |
|---------|-------------|
| `help` | Lista todos los comandos |
| `clear` | Limpia la pantalla |
| `time` | Muestra fecha y hora |
| `mem` | Información de memoria (total/usada/libre) |
| `ps` | Lista procesos (PID, PPID, prioridad, estado, RSP, RBP, foreground) |

### Gestión de procesos
| Comando | Descripción |
|---------|-------------|
| `loop <N>` | Imprime PID cada N segundos (infinito) |
| `kill <pid>` | Mata un proceso |
| `nice <pid> <prio>` | Cambia prioridad (0-4) |
| `block <pid>` | Toggle block/unblock |

### I/O y pipes
| Comando | Descripción |
|---------|-------------|
| `cat` | Lee stdin y escribe a stdout |
| `wc` | Cuenta líneas de stdin |
| `filter` | Filtra vocales de stdin |
| `cmd1 \| cmd2` | Pipe entre dos comandos |
| `cmd &` | Ejecuta en background |

### Sincronización
| Comando | Descripción |
|---------|-------------|
| `mvar <W> <R>` | Producer/consumer con W writers y R readers |

### Tests de la cátedra
| Comando | Descripción |
|---------|-------------|
| `test_mm <bytes>` | Test memory manager (alloc/memset/check/free loop) |
| `test_proc <N>` | Test procesos (create/kill/block/unblock random) |
| `test_sync <N> <use_sem>` | Test sync (1=con semáforos, 0=sin) |
| `test_prio <max_value>` | Test scheduling con prioridades (cuenta de 0 a max_value) |

### Excepciones
| Comando | Descripción |
|---------|-------------|
| `divzero` | Provoca Zero Division Exception |
| `invalidop` | Provoca Invalid Opcode Exception |

## Teclas especiales

| Tecla | Acción |
|-------|--------|
| `Ctrl+C` | Mata el proceso en foreground |
| `Ctrl+D` | Envía EOF (cierra stdin) |

## Arquitectura

### Scheduler
Round Robin con 5 niveles de prioridad (0-4). Quantum = 4 × 2^prioridad ticks. Aging: procesos que esperan 10 quanta son promovidos automáticamente.

### Context Switch
Timer IRQ (IRQ0) → `pushState` → `schedule()` salva/restaura `stack_pos` del PCB → `popState` → `iretq`.

### Semáforos
Named semaphores con `XCHG` atómico para el lock interno. Procesos que no pueden adquirir el semáforo quedan BLOCKED (sin busy-wait).

### Pipes
Buffer circular de 1024 bytes. Lectura/escritura bloqueante. Transparentes a procesos via `file_descriptors[]` routing en `sys_read`/`sys_write`.

### Procesos
Stack de 8192 bytes por proceso. Máximo 20 procesos simultáneos. Estados: READY, RUNNING, BLOCKED, ZOMBIE. Soporte para `waitpid` y cleanup de huérfanos.
