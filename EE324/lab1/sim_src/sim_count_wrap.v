`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/31/2020 02:16:30 PM
// Design Name: 
// Module Name: sim_count_wrap
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module sim_count_wrap(

    );
    
    reg rst, clk, en;
    wire[9:0] a_val, b_val;
    
    wire A, B;
    wire wiz_lock, clkslow;
    
    count_wrap uut (
        .clk(clkslow),
        .rst(rst),
        .a_val(a_val),
        .b_val(b_val),
        .en(en),
        .A(A),
        .B(B)
    );
    
	
	/* clkslow simulation:
	   1 / (8560.713 ns - 8417.856 ns) = 7 MHz
	*/
    clk_wiz_0 inst_clk_wiz
	(
        // Clock out ports
        .clk_out1(clkslow),     // output clk_out1
        // Status and control signals
        .reset(rst), // input reset
        //.locked(wiz_lock),       // output locked
        // Clock in ports
        .clk_in1(clk)		//input clk_in1
	);      

    
    always
        #5 clk = ~clk;
    initial begin
        
        clk = 0;
        en = 0;
        rst = 1;
        //a_val = 0;
        //b_val = 0;
        
        // test out the en/rst
        #10;
        en = 1;
        rst = 0;
        
        #10;
        
        // test out a rst
        #100;
        rst = 1;
        #10;
        rst = 0;
        
        #500;
        
    end
endmodule
