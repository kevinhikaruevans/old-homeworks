----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/22/2020 02:39:11 PM
-- Design Name: 
-- Module Name: sseg_dec - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
USE ieee.numeric_std.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity sseg_dec is
    Port ( value : in STD_LOGIC_VECTOR (3 downto 0);
           c : out STD_LOGIC_VECTOR (7 downto 0);
           dp : in STD_LOGIC);
end sseg_dec;

architecture Behavioral of sseg_dec is

begin
    process(value) begin
        case to_integer(unsigned(value)) is
            when 16#0# => c <= not(dp) & "1000000";
            when 16#1# => c <= not(dp) & "1111001"; 
            when 16#2# => c <= not(dp) & "0100100";
            when 16#3# => c <= not(dp) & "0110000";
            when 16#4# => c <= not(dp) & "0011001";
            when 16#5# => c <= not(dp) & "0010010";
            when 16#6# => c <= not(dp) & "0000010";
            when 16#7# => c <= not(dp) & "1111000";
            when 16#8# => c <= not(dp) & "0000000";
            when 16#9# => c <= not(dp) & "0011000";
            when 16#A# => c <= not(dp) & "0001000";
            when 16#B# => c <= not(dp) & "0000011";
            when 16#C# => c <= not(dp) & "0100111";
            when 16#D# => c <= not(dp) & "0100001";
            when 16#E# => c <= not(dp) & "0000110";
            when 16#F# => c <= not(dp) & "0001110"; 
            when others =>
                c <= "00000000";
        end case;
    end process;

end Behavioral;
