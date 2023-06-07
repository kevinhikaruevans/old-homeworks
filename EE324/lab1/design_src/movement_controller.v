`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/02/2020 08:46:43 PM
// Design Name: 
// Module Name: movement_controller
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


module movement_controller
    #(
        parameter
        DEFAULT_X = 320,
        DEFAULT_Y = 240,
        COUNTERWIDTH = 18
    )
    (
        input clk,
        input rst,
        
        output [9:0] x, y
    );
    
    reg [COUNTERWIDTH:0] counter = 0;
    
    //(* mark_debug = "true", keep = "true" *)
    reg [9:0] _x = DEFAULT_X;
    
    //(* mark_debug = "true", keep = "true" *)
    reg [9:0] _y = DEFAULT_Y;
    
    reg direction = 0;
    
    always @(posedge(clk)) begin
        if (rst) begin
            _x      <= DEFAULT_X;
            _y      <= DEFAULT_Y;
            counter <= 0;
        end
        else if (counter == 0) begin
            if (direction) begin
                _x <= _x + 1;
                _y <= _y + 1;
            
                if (_x == 640 || _y == 480) begin
                    direction <= 0;
                end
            end
            else begin
                _x <= _x - 1;
                _y <= _y - 1;
                
                if (_x == 0 || _y == 0) begin
                    direction <= 1;
                end
            end
        end
        
        counter <= counter + 1;
    end
    
    assign x = _x;
    assign y = _y;
endmodule
