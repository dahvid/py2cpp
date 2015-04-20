/* 
 * What get's supported here 
 * containers -> map, vector, tuple (fusion::vector)
 * basic types -> string, long, double
 * and recursive and variant mixed types of any of the above
 */


/*
Your function works good, but not for nested tuples: the second
overload of tuple_parser::operator() fails to compile because you pass
only one parameter to make_fusion_from_tuple, which takes two.

I got it to work by writing the second overload that way:

template <class Sequence>
typename enable_if<fusion::traits::is_sequence<Sequence>, void>::type
operator() (Sequence& s) const
{
   tuple t;
   add_element(t);
   make_fusion_from_tuple(t, &s);
}

I can then write for instance :

void foo(tuple t)
{
       boost::tuple<int, pair<float, float>, int> bt;
       make_fusion_from_tuple(t, &bt);
       cout << bt.get<0>() << endl;
       cout << bt.get<1>().first << " " << bt.get<1>().second << endl;
       cout << bt.get<2>() << endl;
}

And within python:
> mymodule.foo( (1, (2, 3), 4) )
1
2 3
4
*/


#include <boost/python/py2cpp.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/sequence/comparison/not_equal_to.hpp>
#include <iostream>
#include <tuple>

namespace py = boost::python;
namespace fu = boost::fusion;
using  std::cerr;
using  std::endl;
using  std::cout;
using  std::string;
using  std::size_t;
using  std::get;

const char space = ' ';

//typedef ::vector<fu::vector<string,size_t,string> > BufferList;

int main(int argc, char* argv[])
{
	//if (argc != 3) {
	//	cerr << "test_py2c [module] [graph]" << endl;
	//}
	try {
		::Py_Initialize();
		using namespace py;
		cpp2py cpp_2_py;
		py2cpp py_2_cpp;

		//first try simplest thing 
		{
			typedef fu::vector<long, double, string> Triple;
			Triple t(23, 3.1415, "wot");
			py::tuple o;
			cpp_2_py(t,&o);
			//tuple o = make_tuple_from_fusion(t);
			cout << "made tuple from fusion" << endl;
			Triple empty;

			//py2cpp py_2_cpp(o);
			py_2_cpp(o, &empty);
			//make_fusion_from_tuple(o, &empty);
			cout << "made fusion from tuple" << endl;
			if (empty != t) {
				cerr << "bad two way triple injection extraction" << endl;
				return -1;
			}
			if (argc > 1) {
				string s = convertToString(o);
				cout << s << endl;
			}
		}
		{
			//now try recursion
			typedef fu::vector<long, double, string> Triple;
			typedef fu::vector<long, Triple, string> Rtriple;
			Rtriple r(45, Triple(23,3.23,"wot"),"wit");
			py::tuple o;
			cpp_2_py(r, &o);
			//tuple o = make_tuple_from_fusion(r);
			Rtriple empty;
			//py2cpp py_2_cpp(o);
			py_2_cpp(o, &empty);
			//make_fusion_from_tuple(o, &empty);
			if (empty != r) {
				cerr << "bad two way triple injection extraction 2" << endl;
				return -1;
			}
			if (argc > 1) {
				string s = convertToString(o);
				cout << s << endl;
			}
		}
		{  //now try some standard containers
			std::vector<string> ss,tt;
			ss.push_back("one");
			ss.push_back("two");
			ss.push_back("three");
			py::list l;
			cpp_2_py(ss,&l);
			//list l = make_list_from_vector(ss);

			//py2cpp py_2_cpp(l);
			py_2_cpp(l, &tt);
			//tt = extract<std::vector<string>>(l);
			//make_vector_from_list(l, &tt);
			if (ss != tt){
				cerr << "bad two way vector list injection extraction " << endl;
				return -1;
			}
			if (argc > 1) {
				string s = convertToString(l);
				cout << "vector string conversion " << s << endl;
			}

		}
		{  //now try some standard containers
			std::vector<double> ss,tt;
			ss.push_back(4.3);
			ss.push_back(2.34);
			ss.push_back(1.234);
			py::list l;
			cpp_2_py(ss, &l);
			//list l = make_list_from_vector(ss);

			//tt = extract<std::vector<double>>(l);
			py_2_cpp(l, &tt);
			//make_vector_from_list(l, &tt);
			if (ss != tt){
				cerr << "bad two way vector list injection extraction 2" << endl;
				return -1;
			}
			if (argc > 1) {
				string s = convertToString(l);
				cout << "vector double conversion " << s << endl;
			}

		}
		
		{  //nested
			std::vector<double> ss;
			ss.push_back(4.3);
			ss.push_back(2.34);
			ss.push_back(1.234);

			std::vector<std::vector<double>> dd,ee;
			dd.push_back(ss);
			dd.push_back(ss);
			//dd.push_back(std::vector<std::vector<doubl e>>());
			//list l = make_list_from_vector(dd);
			py::list l;
			cpp_2_py(dd, &l);
			

			py_2_cpp(l, &ee);
			
			//make_vector_from_list(l, &ee);
			if (dd != ee){
				cerr << "bad two way vector list injection extraction 2" << endl;
				return -1;
			}
			 
			if (argc > 1) {
				string s = convertToString(l);
				cout << "nested vector double conversion " << s << endl;
			}
			 
			
		}
		{  //mixed like shit
			typedef fu::vector<long, double, string> Triple;
			typedef std::vector<Triple>    			 TripleVec;
			typedef std::map<string,TripleVec>		 TripleVecMap;
			typedef std::map<long, string> 			 StringIndex;
			typedef fu::vector<Triple, TripleVec, TripleVecMap, StringIndex> Everything;

			Triple triple(123456789, 1.23456789, "one2nine"), empty_triple;
			TripleVec triple_vec, empty_triple_vec;
			triple_vec.push_back(triple);
			triple_vec.push_back(triple);
			triple_vec.push_back(triple);
			TripleVecMap triple_vec_map, empty_triple_vec_map;
			triple_vec_map["one"] = triple_vec;
			triple_vec_map["two"] = triple_vec;
			StringIndex  string_index, empty_string_index;
			string_index[1] = "one";
			string_index[2] = "two";
			Everything everything(triple, triple_vec, triple_vec_map, string_index);
			Everything was_empty;

			//test one by one as we climb up the complexity ladder
			//tuple trip = make_tuple_from_fusion(triple);
			py::tuple trip;
			cpp_2_py(triple, &trip);

			py_2_cpp(trip, &empty_triple);
			//make_fusion_from_tuple(trip, &empty_triple);
			if (triple != empty_triple) {
				cerr << "bad triple conversion" << endl;
				return -1;
			}
			if (argc > 1) {
				string s = convertToString(trip);
				cout << "triple conversion " << s << endl;
			}

			dict vec_map;
			cpp_2_py(triple_vec_map, &vec_map);
			//dict vec_map = make_dict_from_map(triple_vec_map);
			py_2_cpp(vec_map, &empty_triple_vec_map);
			//make_map_from_dict(vec_map, &empty_triple_vec_map);
			if (triple_vec_map != empty_triple_vec_map) {
				cerr << "bad triple_vec_map conversion" << endl;
				return -1;
			}
			if (argc > 1) {
				string s = convertToString(vec_map);
				cout << "nested triple_vec_map conversion " << s << endl;
			}			

			py::tuple every;
			cpp_2_py(everything, &every);
			//tuple every = make_tuple_from_fusion(everything);
			py_2_cpp(every, &was_empty);
			//make_fusion_from_tuple(every, &was_empty);
			if (everything != was_empty){
				cerr << "bad two way mixed balagon extraction" << endl;
				return -1;
			}
			
			if (argc > 1) {
				string s = convertToString(every);
				cout << "everything conversion " << s << endl;
			}
			
			/* 
			//mixed with variant
			//typedef boost::variant<std::vector<string>, Everything> MoreThanEverything;
			std::vector<string> stringy, empty_stringy;
			stringy.push_back("one");
			stringy.push_back("two");
			stringy.push_back("three");
			//load variant, every is alread loaded
			list py_string_list;
			cpp_2_py(stringy,&py_string_list);
			//list string_vec = make_list_from_vector(stringy);


 			boost::PythonVariant var_string_vec, var_everything;
 			
			//c2py as type, py2c as variant, read into variant
			//string_vec and every are py types
			//???once we've read it in, how do we know what it is???
			//MoreThanEverything  string_vec_var;
			//extract string vector into variant
			//PyToCpp  py2c(string_vec);
 			//py2c(&more_than_all);  
 			//std::vector<string> empty_string_vec;
 			py_2_cpp(py_string_list, &var_string_vec);
 			//make_variant_from_object(string_vec, &more_than_all);
 			

			Everything* every_ptr;
			if (every_ptr = get<Everything>(&var_string_vec)) {
				cerr << "got everything type, expected vector of strings" << endl;
				return -1;
			}
			std::vector<string>* vec_ptr;
			if (!(vec_ptr = get<std::vector<string>>(&var_string_vec))) {
				cerr << "failed geting vector of string type" << endl;
				return -1;
			}
			if (*vec_ptr != stringy) {
				cerr << "recovered bad string vector data" << endl;
				return -1;
			}
			*/
			/*
			cout << "about to make tuple variant" << endl;
			make_variant_from_object(every, &more_than_nothing);
			if (!(every_ptr = get<Everything>(&more_than_nothing))) {
				cerr << "failed to get everything type" << endl;
				return -1;
			}
			if (vec_ptr = get<std::vector<string>>(&more_than_all)) {
				cerr << "got vec of string type, expected Everything" << endl;
				return -1;
			}
			
			if (*every_ptr != everything) {
				cerr << "recovered bad everything data" << endl;
				return -1;
			}
			*/
			if (argc > 1)
				cout << "GOT ALL EVERYTHING DATA" << endl;
			
		}
		
	}
	 
    catch (...)
	{      
	     std::cerr << "caught exception" << std::endl;
	     if(PyErr_Occurred())
	     {
	         PyErr_Print();
	     }
	}
	if (argc > 1)
		cout << "DONE" << endl;
}



