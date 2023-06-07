`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/15/2020 09:50:05 AM
// Design Name: 
// Module Name: char_display
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


module char_display(
        input vga_clk,
        input[9:0] vga_x, vga_y,
        input[7:0] ascii_value,
        input[9:0] target_x, target_y,
        output reg on
    );
    
    (* mark_debug = "true" *) 
    wire[15:0] data;
    
    (* mark_debug = "true" *) 
    reg[10:0] addr;
    
    wire [9:0] x_offset = vga_x - target_x;
    wire [9:0] y_offset = vga_y - target_y;
    
    char_rom char_rom_inst (
        .DO(data),       // Output data, width defined by READ_WIDTH parameter
        .ADDR(addr),   // Input address, width defined by read/write port depth
        .CLK(vga_clk),     // 1-bit input clock
        .DI(16'h0),       // Input data port, width defined by WRITE_WIDTH parameter
        .EN(1'b1),       // 1-bit input RAM enable
        .REGCE(1'b0), // 1-bit input output register enable
        .RST(1'b0),     // 1-bit input reset
        .WE(2'd0)        // Input write enable, width defined by write port depth
    );
    
    //assign on = x_offset < 16 ? data[x_offset] : 0;
    always @(posedge(vga_clk)) begin
        if (x_offset < 16 && y_offset < 16)
            on <= data[x_offset];
        else
            on <= 0;
    end
    
    always @(posedge(vga_clk)) begin
        if (x_offset < 16 && y_offset < 16) begin
            addr <= (ascii_value << 4) + y_offset + (x_offset / 16);        
        end else begin
            addr <= 0;
        end
    end
endmodule
