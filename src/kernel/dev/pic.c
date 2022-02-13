#include <sys/io.h>
#include <dev/pic.h>

void pic_init() {
    /* remap pic */
    pic_remap();

    /* disable all interrupts */
    pic_disable_all();

    /* enable slave PIC */
    enable_irq(SLAVE_LINE);
}

void pic_remap() {
    /* cascade mode */
    outb(MASTER_COMMAND_PORT, ICW1);
    io_wait();
    outb(SLAVE_COMMAND_PORT, ICW1);
    io_wait();

    /* offsets */
    outb(MASTER_DATA_PORT, ICW2_MASTER);
    io_wait();
    outb(SLAVE_DATA_PORT, ICW2_SLAVE);
    io_wait();

    /* notify master and slave of each other */
    outb(MASTER_DATA_PORT, ICW3_MASTER);
    io_wait();
    outb(SLAVE_DATA_PORT, ICW3_SLAVE);
    io_wait();

    /* 8086/88 mode */
    outb(MASTER_DATA_PORT, ICW4);
    io_wait();
    outb(SLAVE_DATA_PORT, ICW4);
    io_wait();

    /* disable all interrupts */
    outb(MASTER_DATA_PORT, 0xFF);
    io_wait();
    outb(SLAVE_DATA_PORT, 0xFF);
    io_wait();
}

void pic_disable_all(void) {
    outb(MASTER_DATA_PORT, 0xFF);
    io_wait();
    outb(SLAVE_DATA_PORT, 0xFF);
    io_wait();
}

void pic_disable(void) {
    pic_remap();
    pic_disable_all();
}

void enable_irq(uint8_t irq) {
    if (irq > 15) {
        return;
    }

    uint8_t port;
    if (irq < 8) {
        port = MASTER_DATA_PORT;
    } else {
        port = SLAVE_DATA_PORT;
        irq -= 8;
    }

    uint8_t mask = inb(port) & ~(1 << irq);
    io_wait();
    outb(port, mask);
    io_wait();
}

void disable_irq(uint8_t irq) {
    if (irq > 15) {
        return;
    }

    uint8_t port;
    if (irq < 8) {
        port = MASTER_DATA_PORT;
    } else {
        port = SLAVE_DATA_PORT;
        irq -= 8;
    }

    uint8_t mask = inb(port) | (1 << irq);
    io_wait();
    outb(port, mask);
    io_wait();
}

void send_eoi(uint8_t irq) {
    if (irq > 15) {
        return;
    }

    if (irq >= 8 || irq <= 15) {
        outb(SLAVE_COMMAND_PORT, EOI);
        io_wait();
    }

    outb(MASTER_COMMAND_PORT, EOI);
    io_wait();
} 