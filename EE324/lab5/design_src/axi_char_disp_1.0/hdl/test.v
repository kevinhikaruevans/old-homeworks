`timescale 1ns / 1ps

module char_rom_tb(
    );

    wire [15:0] DO;
    reg pix_clk = 1'b1;
    localparam target = 65;
    
    reg [10:0] addr = 11'd0;
    
    reg [9:0] x = 0, y = 0;
    
    // Generate pix_clk
    always
    begin
        #5 pix_clk <= ~pix_clk;
        x = x + 1;
        
        if (x > 32) begin
            x = 0;
            y = y + 1;
        end
        if (y > 32)
            y = 0;
    end
    
    
    // Increment Address every clock cycle
    always @ (posedge pix_clk)
    begin
        //addr = addr + 1;
        addr = (target << 4) + ((x >> 2) & 1) + (y % 16);
        //addr = target + (x % 16) + (y % 16);
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