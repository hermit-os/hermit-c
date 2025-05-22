project('hermit-c', 'c', default_options: ['warning_level=3', 'b_pie=true'])

thread_dep = dependency('threads')

if host_machine.system() == 'hermit'
    find_program('rustup', required: true)
    cargo = find_program('cargo', required: true)

    arch = host_machine.cpu()
    if get_option('buildtype') == 'debug'
        profile = 'dev'
    else
        profile = 'release'
    endif
    features = 'acpi,dhcpv4,dns,mman,newlib,smp,tcp,udp,virtio-net'
    if arch != 'riscv64'
        features += ',pci'
    endif

    hermit_tgt = custom_target(
        'hermit',
        build_always_stale: true,
        console: true,
        output: 'libhermit.a',
        command: [
            cargo,
            'run',
            '--manifest-path=@SOURCE_ROOT@/kernel/Cargo.toml',
            '--target-dir=@PRIVATE_DIR@/xtask',
            '--package=xtask',
            '--',
            'build',
            '--target-dir=@PRIVATE_DIR@/hermit-kernel',
            '--artifact-dir=@OUTDIR@',
            '--arch',
            arch,
            '--profile',
            profile,
            '--no-default-features',
            '--features',
            features,
        ],
    )

    hermit_dep = declare_dependency(link_args: '-L.', sources: hermit_tgt)
else
    hermit_dep = []
endif

executable('fs', 'src/fs.c', dependencies: hermit_dep)
executable('fstat', 'src/fstat.c', dependencies: hermit_dep)
executable('getaddrinfo', 'src/getaddrinfo.c', dependencies: hermit_dep)
executable('getsockname', 'src/getsockname.c', dependencies: hermit_dep)
executable('hello_world', 'src/hello_world.c', dependencies: hermit_dep)
executable('http_server', 'src/http_server.c', dependencies: hermit_dep)
executable(
    'http_server_poll',
    'src/http_server_poll.c',
    dependencies: hermit_dep,
)
executable(
    'http_server_select',
    'src/http_server_select.c',
    dependencies: hermit_dep,
)
executable('limits', 'src/limits.c', dependencies: hermit_dep)
executable('math', 'src/math.c', dependencies: hermit_dep, link_args: '-lm')
executable('memory', 'src/memory.c', dependencies: hermit_dep)
executable('pthreads', 'src/pthreads.c', dependencies: [hermit_dep, thread_dep])
executable('thread_local', 'src/thread_local.c', dependencies: hermit_dep)
executable('time', 'src/time.c', dependencies: hermit_dep)
executable('uname', 'src/uname.c', dependencies: hermit_dep)
