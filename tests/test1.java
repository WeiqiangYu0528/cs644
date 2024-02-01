public class A {
  public A() {
    
  }
  public int m(int x) {
    for(int a = true; true; a = false){}
    for (i = foo(); true; a = false) {}
    if (true)
	for (;; bar()){ ; }
	else ;
    if (true)
	    for (foo();;) 
		if (false) 
		   ;
		else 
		    ;
	else
	    ;
    if (true)
	    for (;;new String()) 
		if (false) 
		    ;
	else // must bind to closest if 
	   ;

    for (int a = true;;) 
    if (false)
    if (true) 
        ;
    else // must bind to closest if 
        ;
    for (;;)
    for (;;)
        ;
  }
}

public class A {
  public A() {}
  public int m(int x) {
    while (false) {}
  }
}

public class A {
    {
        while(true) {
            {
                    
            }
        }
        while (false) {
	    ;
	    }
        while (true) {
	    if (false) { 
	    }
        while (true) {
	    }
	}
    }
}
