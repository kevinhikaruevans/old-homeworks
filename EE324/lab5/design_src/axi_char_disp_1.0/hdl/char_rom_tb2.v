`timescale 1ns / 1ps


module char_rom_tb2;

wire [15:0] DO;
reg pix_clk = 1'b1;
localparam ascii = 65;
reg [10:0] addr = ascii << 4;
// Generate pix_clk
always
begin
    #5 pix_clk <= ~pix_clk;
end

integer i = 0;

initial begin
    #100;

    for(i = 0; i < 16; i = i + 1) begin
        #20 addr <= addr + 1;
    
    end

    #100 $finish;
end

char_rom char_rom_inst (
    .DO(DO),       // Output data, width defined by READ_WIDTH parameter
    .ADDR(addr),   // Input address, width defined by read/write port depth
    .CLK(pix_clk),     // 1-bit input clock
    .DI(16'h0),       // Input data port, width defined by WRITE_WIDTH parameter
    .EN(1'b1),       // 1-bit input RAM enable
    .REGCE(1'b0), // 1-bit input output register enable
    .RST(1'b0),     // 1-bit input reset
    .WE(2'd0)        // Input write enable, width defined by write port depth
    );

endmodule