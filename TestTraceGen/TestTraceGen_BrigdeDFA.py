from datetime import datetime
import random as rnd
import re
import exrex

regex = "X(ABC)*(YA|A(Y|B|BC)|A|(YA|AY|Y)Z)|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(X(Y|B|YZ)|(XB|BX)(C(ABC)*(YA|A(Y|B|BC)|A|(YA|AY|Y)Z)|C)|B(X|C)|X)|(X(ABC)*YZ|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(BC|(XB|BX)C(ABC)*YZ))(X(ABC)*YZ|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(BC|(XB|BX)C(ABC)*YZ))*(X(ABC)*(YA|A(Y|B|BC)|A|(YA|AY|Y)Z)|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(X(Y|B|YZ)|(XB|BX)(C(ABC)*(YA|A(Y|B|BC)|A|(YA|AY|Y)Z)|C)|B(X|C)|X)|X|A)|(X|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(XB|BX)C|(X(ABC)*YZ|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(BC|(XB|BX)C(ABC)*YZ))(X(ABC)*YZ|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(BC|(XB|BX)C(ABC)*YZ))*(X|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(XB|BX)C))(ABC)*Y|(A|X(ABC)*(YA|AY)Z|(X(ABC)*YZ|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(BC|(XB|BX)C(ABC)*YZ))(X(ABC)*YZ|(A|X(ABC)*(YA|AY)Z)((XY|(XB|BX)C(ABC)*(YA|AY))Z)*(BC|(XB|BX)C(ABC)*YZ))*(A|X(ABC)*(YA|AY)Z))((XY|(XB|BX)C(ABC)*(YA|AY))Z)*B|X|A"

def generate_test_traces_for_brigdeDFA(number_of_failing_traces,length,number_of_passing_traces):
    ''' returns traces '''
    return generate_failing_traces(number_of_failing_traces,length),generate_passing_traces(number_of_passing_traces)

def generate_failing_traces(number_of_failing_traces:int,length:int):
    ''' generates list of failing traces '''
    failing_traces: list = []
    i: int = 0
    #generate input amount of traces
    while len(failing_traces) != int(number_of_failing_traces):
        i = i+1
        trace = ''.join(rnd.choices(["A","B","C","X","Y","Z"],k=int(length))) 
        if not re.match(regex,trace):
            if trace not in failing_traces:
                failing_traces.append(trace)
                
    print(f"it took {i} attempts to creating {number_of_failing_traces} failing")
    return failing_traces

def generate_passing_traces(number_of_passing_traces:int):
    ''' generates list of passing traces '''
    passing_traces: list = []
    i: int = 0
    #generate input amount of traces
    while len(passing_traces) != int(number_of_passing_traces):
        i= i+1
        trace = exrex.getone(regex, limit=2)
        if re.match(regex,trace):
            if trace not in passing_traces:
                passing_traces.append(trace)
                
    print(f"it took {i} attempts to creating {number_of_passing_traces} passing")
    return passing_traces


