// $Id: bigint.cpp,v 1.61 2014-06-26 17:06:06-07 - - $
// Daniel Lavell dlavell@ucsc.edu
// Gavin Mack gmmack@ucsc.edu

#include <string>
#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"

bigint::bigint (long that){//: big_value (that) {
   digit_t digit;
   while(that > 0){
      digit = static_cast<digit_t>(that % 10);
      that = that/10;
      big_value.push_back(digit);
   }
   //DEBUGF ('~', this << " -> " << big_value)
}

bigint::bigint (const string& that){
   auto itor = that.cbegin();
   bool isnegative = false;
   if (itor != that.cend() and *itor == '_') {
      isnegative = true;
      ++itor;
   }
   //cout << "bigint ctor: making bigint?" << endl;
   auto itorator = that.crbegin();
   //int newval = 0;
   digit_t temp;// = '9';
   auto end = that.crend();
   if(isnegative) --end;
   while (itorator != end){
      //cout << "bigint ctor:" << temp << "<- temp" << endl;
///////////////////////////////////////////////////////////////
      temp = *itorator;/////hmmmmmmmm.............///////////
///////////////////////////////////////////////////////////////
      //cout << "bigint ctor:" << temp << "<- temp" << endl;
      //temp = static_cast<digit_t>(that.at(itorator));
      big_value.push_back(temp);
      itorator++;
      // newval = newval * 10 + *itor++ - '0';
   }
   if(isnegative) negative = true;
   else negative = false;
   //DEBUGF ('~', this << " -> " << big_value)
}

using digit_t = unsigned char;
using bigvalue_t = vector<digit_t>;

bigvalue_t do_bigadd(const bigvalue_t& left, 
         const bigvalue_t& right){   // supposed tp be private
         //needs to return bigvalue_t... not too sure this will work
   bigvalue_t result{};// = left;
   int maxsize;
   int maxl = left.size();
   int maxr = right.size();
   if(maxl > maxr) maxsize = maxl;
   else maxsize = maxr;
   //initialize variables
   digit_t num;
   int left_num, right_num, carry, sum;
   auto left_itor  = left.cbegin();
   auto right_itor = right.cbegin();
   // loop through each digit and put their sum into the result
   for(int j = 0; j < maxsize; ++j){
      // in case left number has less digits
      //if(j < maxl and j < maxr) break;
      if(j < maxl){ 
         num = *left_itor;
         left_num = num - '0';
         ++left_itor;
      }else{
         num = '0';
         left_num = num - '0';
      }
      // in case right number has less digits
      if(j < maxr){
         num = *right_itor; 
         right_num = num - '0';
         ++right_itor;
      }else{
         num = '0';
         right_num = num - '0';
      }
      // deal with individual digit
      sum = (left_num + right_num + carry);// - '0';
      if(sum > 9){
         carry = 1;
         sum -= 10;
         num = sum + '0';
      }else{
         carry = 0;
         num = sum + '0';
      }
      result.push_back(num);// mod 10 on char?
      sum = 0;
   }
   if(carry == 1){
      num = carry + '0';
      result.push_back(num);
   }
   // not yet made..
   // trim_zeros(result);
   return result;
   //return 2; 
}


// ALMOST IDENTICAL TO D0_bIgAdD
bigvalue_t do_bigsub(const bigvalue_t& left,
         const bigvalue_t& right){
   bigvalue_t result = left;
   //.... might work
   // initialize variables
   digit_t num, left_num, right_num, carry;
   signed char temp;
   size_t i = 0;
   auto itor       = result.cbegin();
   auto left_itor  = left.cbegin();
   auto right_itor = right.cbegin();
   // loop through each digit and put their sum into the result
   while(itor != result.cend()){
      // in case left number has less digits
      if(left.size() < i) left_num = '0' - '0';
      else left_num = *left_itor - '0';
      // in case right number has less digits
      if(right.size() < i) right_num = '0' - '0';
      else right_num = *right_itor - '0';
      // deal with individual digit
      /////////////////////////////////////////////////////////////////
      if(carry != '0') left_num = left_num - ('1' - '0');//// can you 
                                         //substrationate chars
      temp = (left_num - right_num);// - '0';
      carry = '0' - '0';
      if(temp < '0' - '0'){
         carry = '1' - '0';              //main differences are here...
         num = temp + 10;
      }
      num = temp;
      result.push_back(num);
      /////////////////////////////////////////////////////////////////
   }
   return result;
}

bigvalue_t do_bigmul(const bigvalue_t& left, const bigvalue_t& right){

   int carry = 0;
   digit_t digit = 0;
   bigvalue_t prod;
   prod.reserve(left.size() + right.size());
   for (size_t i=0; i<left.size(); ++i) {
      carry = 0;
      for (size_t j=0; j<right.size(); ++j) {
         digit = prod.at(i+j) + left.at(i)*right.at(j) + carry;
         prod.at(i+j) = digit % 10;
         carry = digit / 10;
      }
      prod.at(i+right.size()) = carry;
   }
   return prod;
}

bool do_bigless(const bigvalue_t& left, const bigvalue_t& right){
   auto l_itor = left.crbegin();
   auto r_itor = right.crbegin();
   if (left.size()<right.size()){return true;}
   else if (left.size()>right.size()){return false;}
   while (l_itor != left.crend()) {
      if (*l_itor-'0' < *r_itor-'0'){return true;}
      else if (*l_itor-'0' > *r_itor-'0'){return false;}
      ++l_itor;
      ++r_itor;
   }
   return false;
}

bigint operator+ (const bigint& left, const bigint& right) {
   bigint sum{};
   if(left == right){
      cout << "op+ l==r" << endl;
      sum.negative = left.negative;
      sum.big_value = do_bigadd(left.big_value, right.big_value);
   }else if(left.negative == right.negative){
      sum.negative = left.negative;
      sum.big_value = do_bigadd(left.big_value, right.big_value);
   }else if(do_bigless(left.big_value, right.big_value)){
      sum.negative = right.negative;
      sum.big_value = do_bigsub(right.big_value, left.big_value);
   }else{
      sum.negative = left.negative;
      sum.big_value = do_bigsub(left.big_value, right.big_value);
   }  
   return sum;
}


bigint operator- (const bigint& left, const bigint& right) {
   bigint sum{};
      // should work...
   if(left == right){
      sum.negative = left.negative;
      sum.big_value = do_bigsub(left.big_value, right.big_value);
   }else if(left.negative == right.negative){
      sum.negative = left.negative;
      sum.big_value = do_bigsub(left.big_value, right.big_value);
   }else if(do_bigless(left.big_value, right.big_value)){
      sum.negative = right.negative;
      sum.big_value = do_bigsub(right.big_value, left.big_value);
   }else{
      sum.negative = left.negative;
      sum.big_value = do_bigsub(left.big_value, right.big_value);
   }  
    
   return sum;
}
/*
bigint bigint::operator+ (const bigint& right) {
   
   bigint sum{};
   
   if(this->big_value == right.big_value){
      sum.negative = this->negative;
      sum.big_value = do_bigadd(this->big_value, right.big_value);
   }else if(this->negative == right.negative){
      sum.negative = this->negative;
      sum.big_value = do_bigadd(this->big_value, right.big_value);
   }else if(do_bigless(this->big_value, right.big_value)){
      sum.negative = right.negative;
      sum.big_value = do_bigsub(right.big_value, this->big_value);
   }else{
      sum.negative = this->negative;
      sum.big_value = do_bigsub(this->big_value, right.big_value);
   }  
   
   return sum;
}

bigint operator- (const bigint& right) {
   return -right.long_value;
}
*/
long bigint::to_long() const {
   long result;
   auto itor = this->big_value.cbegin();
   auto end = this->big_value.cend();
   int i = 0;
   for(; itor != end; ++itor){
      result = *itor * 10^i;
      i++;
   }
   if (*this <= bigint (numeric_limits<long>::min())
    or *this > bigint (numeric_limits<long>::max()))
               throw range_error ("bigint::to_long: out of range");
   return result;
}

bool abs_less (const bigvalue_t& left, const bigvalue_t& right) {
   return do_bigless(left, right);
}

//
// Multiplication algorithm.
//
bigint operator* (const bigint& left, const bigint& right) {
   bigint result{};
   if (left.negative or right.negative) result.negative = true;
   if (left.negative and right.negative) result.negative = false;
   result.big_value = do_bigmul(left.big_value, right.big_value);
   return result;
}

//
// Division algorithm.
//

void multiply_by_2 (bigint::bigvalue_t& big_value) {
   bigvalue_t two;
   digit_t char_two= '2';
   two.push_back(char_two);
   big_value = do_bigmul(big_value, two);
   //unumber_value *= 2;
}

void divide_by_2 (bigint::bigvalue_t& big_value) {
   bigint one = 1;
   bigint two = 2;
   bigint quotient;
   bigint remainder; 
   remainder.big_value = big_value;
   while(remainder > two){
      remainder = remainder - two;
      quotient = quotient + one;
   }
   big_value = quotient.big_value;
}


bigint::quot_rem divide (const bigint& left, const bigint& right) {
   //digit_t zero_char = '0';
   //digit_t one_char = '1';
   static bigint zero = 0;
   //zero.push_back(zero_char);
   if (right.big_value == zero.big_value)
        throw domain_error ("divide by 0");
   if (right.big_value == zero.big_value) 
        throw domain_error ("bigint::divide");
   //////////
   bigint divisor;
   divisor.big_value = right.big_value;
   bigint quotient = 0;
   bigint power_of_2 = 1;
   bigint remainder;
   remainder.big_value = left.big_value;
   //bigvalue_t divisor = right.big_value;
   //bigvalue_t quotient;
   /////////
   //quotient.push_back(zero_char);
   //bigvalue_t remainder = left.big_value;
   //bigvalue_t power_of_2;
   //power_of_2.push_back(one_char);
   while (abs_less (divisor.big_value, remainder.big_value)) {
      multiply_by_2 (divisor.big_value);
      multiply_by_2 (power_of_2.big_value);
   }
   while (abs_less (zero.big_value, power_of_2.big_value)) {
      if (not abs_less (remainder.big_value, divisor.big_value)) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divide_by_2 (divisor.big_value);
      divide_by_2 (power_of_2.big_value);
   }
   bigint quot_ret = quotient;
   bigint remain_ret = remainder;
   if(left.negative or right.negative){
      quot_ret.negative = true;
   }
   if(left.negative and right.negative){
      quot_ret.negative = false;
   }
   return {quot_ret, remain_ret};
}

bigint operator/ (const bigint& left, const bigint& right) {
   return divide (left, right).first;
}

bigint operator% (const bigint& left, const bigint& right) {
   return divide (left, right).second;
}


//works
bool operator== (const bigint& left, const bigint& right) {
   if (left.big_value.size() != right.big_value.size()) return false;
   if (left.negative != right.negative) return false;
   auto l_itor = left.big_value.cbegin();
   auto r_itor = right.big_value.cbegin();
   while (l_itor != left.big_value.cend()) {
      if (*l_itor-'0' != *r_itor-'0') return false;
      ++l_itor;
      ++r_itor;
   }
   return true;
}

bool operator< (const bigint& left, const bigint& right) {
   if (left.negative == right.negative) {
      if (!left.negative) {
         if (left.big_value.size() < right.big_value.size()) {
            return true;
         }else if (right.big_value.size() < left.big_value.size()) {
            return false;
         }
         return do_bigless(left.big_value, right.big_value);
      }
      if (left.negative) {
         if (left.big_value.size() < right.big_value.size()) {
            return false;
         }else if (right.big_value.size() < left.big_value.size()) {
            return true;
         }
         return do_bigless(right.big_value, left.big_value);
      }
   }
   if (left.negative) return true;
   return false;
}

ostream& operator<< (ostream& out, const bigint& that) {
   auto itor = that.big_value.end();
   --itor;
   if(that.negative) out << "-";
   int count = 0;
   unsigned char cur;
   //cout << "op<< before loop" << endl; 
   for(int i = that.big_value.size() - 1; i >= 0; --i){
      count++;
      if(count == 70){
         out << "\\" << endl;
         count = 1;
      }
      //cout << "op<< in loop" << endl;
      cur = *itor;
      out << cur;
      --itor;
   }
   
   return out;
}


bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) return 0;
   bigint base_copy = base;
   long expt = exponent.to_long();
   bigint result = 1;
   if (expt < 0) {
      base_copy = 1 / base_copy;
      expt =  - expt;
   }
   while (expt > 0) {
      if (expt & 1) { //odd
         result = result * base_copy;
         --expt;
      }else { //even
         base_copy = base_copy * base_copy;
         expt /= 2;
      }
   }
   DEBUGF ('^', "result = " << result);
   return result;
}




