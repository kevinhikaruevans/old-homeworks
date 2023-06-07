`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/25/2020 02:26:08 PM
// Design Name: 
// Module Name: pwm_controller
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


module pwm_controller
    #(
        parameter COUNTER_SIZE = 10
        //parameter clk_div = 1000 
    )
    (
        input clk_100M,
        (* mark_debug = "true" *)
        input reset,
        input enable,
        
        input [COUNTER_SIZE - 1:0] clk_div,
        (* mark_debug = "true" *)
        input [COUNTER_SIZE - 1:0] pwm_value,
        
        (* mark_debug = "true" *)
        output pwm_out
    );
    
    (* mark_debug = "true" *)
    reg[COUNTER_SIZE - 1:0] clk_counter;
    
    (* mark_debug = "true" *)
    reg[COUNTER_SIZE - 1:0] pwm_counter;
    
    (* mark_debug = "true" *)
    reg clk_slow;
    
    assign pwm_out = enable && (pwm_counter < pwm_value);
    
    // reduce 100 MHz to a slower clock "clk_slow"
    always @(posedge(clk_100M), posedge(reset)) begin
        if (reset == 1) begin
            clk_counter <= 0;
            clk_slow <= 0;
        end
        else if (clk_counter == clk_div) begin
            clk_counter <= 0;
            clk_slow <= !clk_slow;
        end
        else
            clk_counter <= clk_counter + 1;
    end
    
    // increment pwm_counter from 0 to 2^10 - 1
    always @(posedge(clk_slow), posedge(reset)) begin
        if (reset == 1) begin
            pwm_counter <= 0;
        end
        else
            pwm_counter <= pwm_counter + 1;    
    end
endmodule
