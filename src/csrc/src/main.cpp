#include "tet.h"
#include "utils/macro.h"
#include "utils/debug.h"

// #define MAX_SIM_TIME 50 最大仿真周期
vluint64_t sim_time = 0;

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
static Vtop* top;

int tet_step = 1; // 记录一共走了多少步，出错时抛出，方便单步调试到周围 

//================ SIM FUNCTION =====================//
void step_and_dump_wave() {
    top->eval();
    contextp->timeInc(1);
    tfp->dump(contextp->time());
    sim_time++;
}

void sim_init() {
    contextp = new VerilatedContext;
    tfp = new VerilatedVcdC;
    top = new Vtb;

    contextp->traceEverOn(true);
    top->trace(tfp, 0);
    tfp->open("dump.vcd");

    top->reset = 1; top->clock = 0; step_and_dump_wave();
    top->reset = 1; top->clock = 1; step_and_dump_wave();
    top->reset = 0; top->clock = 0; step_and_dump_wave();   
} // 低电平复位

void tet_exec_once() {
    top->clock ^= 1; step_and_dump_wave();
    top->clock ^= 1; step_and_dump_wave();
    // dump_gpr(); 
    // npc_step++;
} // 翻转两次走一条指令

void sim_exit() {
    contextp->timeInc(1);
    tfp->dump(contextp->time());
    tfp->close();
    printf("The wave data has been saved to the dump.vcd\n");
}



//================ main =====================//
int main(int argc, char *argv[]) {
    sim_init();

    init_monitor(argc, argv);

    sdb_mainloop();

    sim_exit();

} 