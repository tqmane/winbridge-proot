/* SPDX-License-Identifier: GPL-2.0-or-later
 * Compile without an i386 libc: clang -m32 -ffreestanding -fno-stack-protector
 *   -nostdlib -static -Wl,-e,_start test-socketcall32.c -o test-socketcall32
 * Run with a 64-bit PRoot: proot -0 ./test-socketcall32
 * SCM_RIGHTS checks decoding; SCM_CREDENTIALS also checks pointer write-back.
 */
typedef unsigned int u32;

static int syscall3(int number, u32 a, u32 b, u32 c)
{
	int result;
	__asm__ volatile("int $0x80" : "=a"(result) : "0"(number), "b"(a), "c"(b), "d"(c) : "memory", "cc");
	return result;
}

void _start(void)
{
	int sockets[2];
	u32 pair[] = { 1, 1, 0, (u32) sockets };
	char byte = 'x';
	u32 iov[] = { (u32) &byte, 1 };
	u32 control[] = { 16, 1, 1, 0, 0, 0 };
	u32 message[] = { 0, 0, (u32) iov, 1, (u32) control, 16, 0 };
	u32 send[] = { 0, (u32) message, 0 };
	int result = syscall3(102, 8, (u32) pair, 0); /* socketpair */
	if (result != 0) syscall3(1, 10, 0, 0);
	send[0] = sockets[0];
	control[3] = sockets[0];
	if (syscall3(102, 16, (u32) send, 0) != 1) syscall3(1, 11, 0, 0);
	control[0] = 24;
	control[2] = 2;
	control[3] = syscall3(20, 0, 0, 0); /* getpid */
	message[5] = 24;
	if (syscall3(102, 16, (u32) send, 0) != 1) syscall3(1, 12, 0, 0);
	syscall3(1, 0, 0, 0);
	__builtin_unreachable();
}
