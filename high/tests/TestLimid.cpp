#include "pnlHigh.hpp"
#include "pnlException.hpp"
#include "pnl_dll.hpp"
#include <string>

std::ostream &operator<<(std::ostream &str, TokArr &ta)
{
    return str << String(ta);
}

OilTest()
{
    LIMID *net;

    net = new LIMID();
    TokArr aChoice = "False True";// possible values for nodes
    TokArr aCh = "False True MayBe";// possible values for nodes
    
    TokArr aIncome = "Cost";// possible values for nodes   

    net->AddNode(chance ^"h1", aCh);
    net->AddNode(decision^"d1", aChoice);
    net->AddNode(chance ^ "h2", aCh);
    net->AddNode(decision^"d2", aChoice);

    net->AddNode(value^"u1", aIncome);
    net->AddNode(value^"u2", aIncome);

    net->AddArc("h1", "h2");
    net->AddArc("h1", "u1");
    net->AddArc("d1", "u2");
    net->AddArc("d1", "h2");
    net->AddArc("d2", "u1");
    net->AddArc("h2", "d2");

    net->SetPChance("h1^False h1^True h1^MayBe", "0.5 0.3 0.2");

    net->SetPDecision("d1^False d1^True", "0.5 0.5");

    net->SetPChance("h2^False h2^True h2^MayBe", "0.1 0.3 0.6", "h1^False d1^False");
    net->SetPChance("h2^False h2^True h2^MayBe", "0.333333 0.333333 0.333333", "h1^False d1^True");
    net->SetPChance("h2^False h2^True h2^MayBe", "0.3 0.4 0.3", "h1^True d1^False");
    net->SetPChance("h2^False h2^True h2^MayBe", "0.333333 0.333333 0.333333", "h1^True d1^True");
    net->SetPChance("h2^False h2^True h2^MayBe", "0.5 0.4 0.1", "h1^MayBe d1^False");
    net->SetPChance("h2^False h2^True h2^MayBe", "0.333333 0.333333 0.333333", "h1^MayBe d1^True");
    
    net->SetPDecision("d2^False d2^True", "0.5 0.5", "h2^False");
    net->SetPDecision("d2^False d2^True", "0.5 0.5", "h2^True");
    net->SetPDecision("d2^False d2^True", "0.5 0.5", "h2^MayBe");
 
    net->SetValueCost("u1^Cost", "-70000.0", "h1^False d2^False");
    net->SetValueCost("u1^Cost", "0.0", "h1^False d2^True");
    net->SetValueCost("u1^Cost", "50000.0", "h1^True d2^False");
    net->SetValueCost("u1^Cost", "0.0", "h1^True d2^True");
    net->SetValueCost("u1^Cost", "200000.0", "h1^MayBe d2^False");
    net->SetValueCost("u1^Cost", "0.0", "h1^MayBe d2^True");

    net->SetValueCost("u2^Cost", "-10000.0", "d1^False");
    net->SetValueCost("u2^Cost", "0.0", "d1^True");

    TokArr exp;
    exp = net->GetExpectation();
    cout << exp;
    cout <<"\n";

    TokArr politics;
    politics = net->GetPolitics();
    cout << politics;
    cout <<"\n";

    delete net;
}


TestPigs()
{
    LIMID *net;

    net = new LIMID();
    TokArr aChoice = "False True";// possible values for nodes
    
    TokArr aIncome = "Cost";// possible values for nodes

    net->AddNode(chance^"h1", aChoice);
    net->AddNode(chance^"t1", aChoice);
    net->AddNode(decision^"d1", aChoice);

    net->AddNode(chance^"h2", aChoice);
    net->AddNode(chance^"t2", aChoice);
    net->AddNode(decision^"d2", aChoice);

    net->AddNode(chance^"h3", aChoice);
    net->AddNode(chance^"t3", aChoice);
    net->AddNode(decision^"d3", aChoice);

    net->AddNode(chance^"h4", aChoice);

    net->AddNode(value^"u1", aIncome);
    net->AddNode(value^"u2", aIncome);
    net->AddNode(value^"u3", aIncome);
    net->AddNode(value^"u4", aIncome);
    
    net->AddArc("h1", "h2");
    net->AddArc("h1", "t1");
    net->AddArc("h2", "t2");
    net->AddArc("t1", "d1");
    net->AddArc("t2", "d2");
    net->AddArc("d1", "h2");
    net->AddArc("h3", "t3");
    net->AddArc("t3", "d3");
    net->AddArc("d2", "h3");
    net->AddArc("h2", "h3");
    net->AddArc("h3", "h4");
    net->AddArc("d3", "h4");
    net->AddArc("d1", "u1");
    net->AddArc("d2", "u2");
    net->AddArc("d3", "u3");
    net->AddArc("h4", "u4");

    net->SetPChance("h1^False h1^True", "0.9 0.1");

    net->SetPChance("t1^False t1^True", "0.1 0.9", "h1^False");
    net->SetPChance("t1^False t1^True", "0.8 0.2", "h1^True");

    net->SetPDecision("d1^False d1^True", "0.5 0.5", "t1^False");
    net->SetPDecision("d1^False d1^True", "0.5 0.5", "t1^True");

    net->SetValueCost("u1^Cost", "-100.0", "d1^False");
    net->SetValueCost("u1^Cost", "0.0", "d1^True");

    net->SetPChance("h2^False h2^True", "0.9 0.1", "h1^False d1^False");
    net->SetPChance("h2^False h2^True", "0.8 0.2", "h1^False d1^True");

    net->SetPChance("h2^False h2^True", "0.5 0.5", "h1^True d1^False");
    net->SetPChance("h2^False h2^True", "0.1 0.9", "h1^True d1^True");

    net->SetPChance("t2^False t2^True", "0.1 0.9", "h2^False");
    net->SetPChance("t2^False t2^True", "0.8 0.2", "h2^True");

    net->SetPDecision("d2^False d2^True", "0.5 0.5", "t2^False");
    net->SetPDecision("d2^False d2^True", "0.5 0.5", "t2^True");

    net->SetValueCost("u2^Cost", "-100.0", "d2^False");
    net->SetValueCost("u2^Cost", "0.0", "d2^True");

    net->SetPChance("h3^False h3^True", "0.9 0.1", "h2^False d2^False");
    net->SetPChance("h3^False h3^True", "0.5 0.5", "h2^False d2^True");

    net->SetPChance("h3^False h3^True", "0.8 0.2", "h2^True d2^False");
    net->SetPChance("h3^False h3^True", "0.1 0.9", "h2^True d2^True");

    net->SetPChance("t3^False t3^True", "0.1 0.9", "h3^False");
    net->SetPChance("t3^False t3^True", "0.8 0.2", "h3^True");

    net->SetPDecision("d3^False d3^True", "0.5 0.5", "t3^False");
    net->SetPDecision("d3^False d3^True", "0.5 0.5", "t3^True");

    net->SetValueCost("u3^Cost", "-100.0", "d3^False");
    net->SetValueCost("u3^Cost", "0.0", "d3^True");

    net->SetPChance("h4^False h4^True", "0.9 0.1", "h3^False d3^False");
    net->SetPChance("h4^False h4^True", "0.8 0.2", "h3^False d3^True");

    net->SetPChance("h4^False h4^True", "0.5 0.5", "h3^True d3^False");
    net->SetPChance("h4^False h4^True", "0.1 0.9", "h3^True d3^True");

    net->SetValueCost("u4^Cost", "1000.0", "h4^False");
    net->SetValueCost("u4^Cost", "300.0", "h4^True");

    net->SaveNet("pigs.xml");

    LIMID *newNet;
    newNet = new LIMID();

    newNet->LoadNet("pigs.xml");

//    newNet->SetIterMax(5);

    TokArr exp;
    exp = newNet->GetExpectation();
    cout << exp;
    cout <<"\n";

/*    TokArr politics;
    politics = net->GetPolitics();
    cout << politics;
    cout <<"\n";
*/
//    delete net;
}

int main(char* argv[], int argc)
{
	bool IsTestOK = true;
	
	try 
	{
            TestPigs();
//            OilTest();
	}
	catch(pnl::CException e)
	{
		std::cout << e.GetMessage();
	}
	
	return 0;
}
