#pragma once  

#ifdef MATHLIBRARY_EXPORTS  
#define MATHLIBRARY_API __declspec(dllexport)   
#else  
#define MATHLIBRARY_API __declspec(dllimport)   
#endif

namespace IProcess {
	class Functions
	{
	public:
		// Returns a + b  
		static MATHLIBRARY_API double Add(double a, double b);

		// Returns a * b  
		static MATHLIBRARY_API double Multiply(double a, double b);

		// Returns a + (a * b)  
		static MATHLIBRARY_API double AddMultiply(double a, double b);
	};
}


