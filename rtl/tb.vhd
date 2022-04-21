---------------------------------------------------------------------------------------------------
-- Simple FLI demo calling the Python Numpy library from VHDL
--
-- https://github.com/htminuslab            
--                                       
---------------------------------------------------------------------------------------------------
--                                                    
---------------------------------------------------------------------------------------------------
--
--  Testbench creating 2 sine waves then calls numpy to calculate FFT
-- 
--  Revision History:                                                        
--                                                                           
--  Date:          Revision         Author         
--  30 Nov 2017    0.1              Hans Tiggeler 
---------------------------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package pkg is

	type real_array is array(1 to 500) of real;

	procedure call_python_module(module_name:in string);
	attribute FOREIGN of call_python_module : procedure is "call_python_module ./fli_python.dll";	
		
	procedure call_python_function(function_name:in string; vhdl_array_in:in real_array; vhdl_array_out:out real_array);
	attribute FOREIGN of call_python_function : procedure is "call_python_function ./fli_python.dll";			
end;

package body pkg is	
	procedure call_python_module(module_name:in string) is begin end;
	procedure call_python_function(function_name:in string; vhdl_array_in:in real_array; vhdl_array_out:out real_array) is begin end;
end;


entity tb is
end tb;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;
use std.textio.all;

use work.pkg.all;

architecture example of tb is

begin

  process
	 variable sinwave1,sinwave2 : real;
	 variable vhdl_array_in  : real_array;
	 variable vhdl_array_out : real_array;
	 variable noise  : real;
	 variable seed1, seed2 : integer := 999;
	 variable ln     : line; 
  begin
  
	call_python_module("numpy_test");

	for i in 1 to 500 loop       							-- 500 samples
		sinwave1:=sin(3.0*MATH_PI*2.0*real(i)/100.0);		-- 15Hz
		sinwave2:=sin(6.0*MATH_PI*2.0*real(i)/100.0);		-- 30Hz
		
		UNIFORM(seed1, seed2, noise);						-- Add some noise
		vhdl_array_in(i):=sinwave1+sinwave2+noise*3.0;		-- Add to input real array
    end loop;
	
	call_python_function("do_fft",vhdl_array_in,vhdl_array_out);-- Calc FFT		
	wait for 100 ns;
	
	for i in 1 to 10 loop  									-- Only display first 10 real results     
		write(ln,"FFT("& to_string(i) &")=" & to_string(vhdl_array_out(i))); 		
		writeline(OUTPUT,ln);		
    end loop;

	wait;
  end process;

end;

