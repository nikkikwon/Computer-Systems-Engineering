/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);
    
    outb(ICW2_MASTER, 1 + MASTER_8259_PORT);
    outb(ICW2_SLAVE, 1 + SLAVE_8259_PORT);
    outb(ICW3_MASTER, 1 + MASTER_8259_PORT);
    outb(ICW3_SLAVE, 1 + SLAVE_8259_PORT);
    outb(ICW4_MASTER, 1 + MASTER_8259_PORT);
    outb(ICW4_SLAVE, 1 + SLAVE_8259_PORT);

    enable_irq(slave_irq_num);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    /* invalid parameter 'irq_num' */
    if(irq_num>15){return;}
    if(irq_num<0){return;}

    uint8_t mask = 0xFE;//initial mask
    int i;//as an index
    if((irq_num >= 0) && (irq_num <= 7)){
        for(i=0; i<irq_num; i++){
            /* we move the '0' in mask to the index 'irq_num'
                for example, if irq_num is 0, we don't move, mask is just 0xFE */
            mask = mask<<1;
            mask += 1;
        }
        /* then set this answer to the master_mask */
        master_mask &= mask;
        outb(master_mask, 1 + MASTER_8259_PORT);
    }
    else{
        /* this case, irq_num is in [8,15], it shows this irq_unm is in slave region */
        irq_num = irq_num - 8;
        for(i = 0; i<irq_num; i++){
            /* we move the '0' in mask to the index 'irq_num'
                for example, if irq_num is 0, we don't move, mask is just 0xFE */
                mask = mask<<1;
                mask+=1; 
        }
        /* then set this answer to the slave_mask */
        slave_mask &= mask;
        outb(slave_mask, 1 + SLAVE_8259_PORT);
        master_mask &= 0xFB;//unmask the master 2.
        outb(master_mask, 1 + MASTER_8259_PORT);
    }
    return;
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    /* this functoin is nothing but only the mask is different with the function 'enable_irq',
    in 'enable_irq' we find the right place and set it to 0, in this 'disable_irq', we put 1 there */

   /* invalid parameter 'irq_num' */
    if(irq_num>15){return;}
    if(irq_num<0){return;}

    uint8_t mask = 0x01;//initial mask
    int i;//as an index
    if((irq_num >= 0) && (irq_num <= 7)){
        for(i=0; i<irq_num; i++){
            /* we move the '0' in mask to the index 'irq_num'
                for example, if irq_num is 0, we don't move, mask is just 0xFE */
            mask = mask<<1;
            mask += 1;
        }
        /* then set this answer to the master_mask */
        master_mask |= mask;
        outb(master_mask, 1 + MASTER_8259_PORT);
    }
    else{
        /* this case, irq_num is in [8,15], it shows this irq_unm is in slave region */
        irq_num = irq_num - 8;
        for(i = 0; i<irq_num; i++){
            /* we move the '0' in mask to the index 'irq_num'
                for example, if irq_num is 0, we don't move, mask is just 0xFE */
                mask = mask<<1;
                //mask+=1; 
        }
        /* then set this answer to the slave_mask */
        slave_mask |= mask;
        outb(slave_mask, 1 + SLAVE_8259_PORT);
    }
    return;
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    /* EOI at first is 0x60 */

    /* invalid irq_num */
    if(irq_num>15){return;}
    if(irq_num<0){return;}

    if((irq_num >=0) && (irq_num<=7)){
        /* this irq_num is in master region */
        outb(EOI | irq_num, MASTER_8259_PORT);
    }
    else{
        /* this irq_num is in slave region */
        irq_num = irq_num-8;
        outb(EOI | irq_num, SLAVE_8259_PORT);
        outb(EOI + 2, MASTER_8259_PORT);
    }

}
