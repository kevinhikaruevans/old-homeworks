----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/22/2020 02:46:33 PM
-- Design Name: 
-- Module Name: sseg_controller - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity sseg_controller is
  Port (
    clk : in std_logic;
    value : in std_logic_vector(13 downto 0);
    --btn : in std_logic;
    c : out std_logic_vector(7 downto 0);
    an : out std_logic_vector(3 downto 0)
  );
end sseg_controller;

architecture Behavioral of sseg_controller is
    component sseg_dec is
        Port ( value : in STD_LOGIC_VECTOR (3 downto 0);
               c : out STD_LOGIC_VECTOR (7 downto 0);
               dp : in STD_LOGIC);
    end component sseg_dec;
    
    signal current_digit : std_logic_vector(3 downto 0) := "0001";
    signal value0, value1, value2, value3 : std_logic_vector(3 downto 0);
    signal cathode0, cathode1, cathode2, cathode3 : std_logic_vector(7 downto 0);
begin
    
    seg0 : component sseg_dec
        port map(
            value => value0,
            c => cathode0,
            dp => '0'
        );
    
    seg1 : component sseg_dec
        port map(
            value => value1,
            c => cathode1,
            dp => '0'
        );
    seg2 : component sseg_dec
        port map(
            value => value2,
            c => cathode2,
            dp => '0'
        );
    seg3 : component sseg_dec
        port map(
            value => value3,
            c => cathode3,
            dp => '1'
        );        
      
    
    get_digit_values : process (clk) begin
        if rising_edge(clk) then
            value0 <= std_logic_vector( resize( unsigned(value) mod 10, value0'length) );
            value1 <= std_logic_vector( resize( (unsigned(value) / 10) mod 10, value1'length) );
            value2 <= std_logic_vector( resize( (unsigned(value) / 100) mod 10, value2'length) );
            value3 <= std_logic_vector( resize( (unsigned(value) / 1000) mod 10, value3'length) ); 
        end if;
    end process;
    
    digit_ring_counter : process(clk) begin
        if rising_edge(clk) then
            -- I don't understand how shifting works in vhdl tbh
            current_digit(1) <= current_digit(0);
            current_digit(2) <= current_digit(1);
            current_digit(3) <= current_digit(2);
            current_digit(0) <= current_digit(3);
        end if;
    end process;
    
    
    cycle_sseg : process(clk) begin
        if rising_edge(clk) then
            case current_digit is
                when "1000" => c <= cathode3;
                when "0100" => c <= cathode2;
                when "0010" => c <= cathode1;
                when "0001" => c <= cathode0;
                when others => c <= cathode0;
            end case;
            
            an <= not current_digit;
        end if;
    end process;
    
    --an <= not current_digit;
    --an <= not std_logic_vector(shift_left(unsigned("1"), unsigned(current_digit)));
    
    
end Behavioral;
