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
    features = 'acpi,dhcpv4,newlib,smp,tcp'
    if arch != 'riscv64'
        features += ',pci'
    endif

    hermit = custom_target(
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
else
    hermit = []
endif

executable('fs', 'src/fs.c', link_whole: hermit)
executable('fstat', 'src/fstat.c', link_whole: hermit)
executable('hello_world', 'src/hello_world.c', link_whole: hermit)
executable('http_server', 'src/http_server.c', link_whole: hermit)
executable('http_server_poll', 'src/http_server_poll.c', link_whole: hermit)
executable('limits', 'src/limits.c', link_whole: hermit)
executable('math', 'src/math.c', link_whole: hermit)
executable('memory', 'src/memory.c', link_whole: hermit)
executable(
    'pthreads',
    'src/pthreads.c',
    dependencies: thread_dep,
    link_whole: hermit,
)
executable('thread_local', 'src/thread_local.c', link_whole: hermit)
executable('time', 'src/time.c', link_whole: hermit)
executable('uname', 'src/uname.c', link_whole: hermit)
