#include <cstring>
#include <fstream>
#include <iostream>
#include <variant>

#include "parser_types.hpp"
#include "read_data.hpp"

namespace simplex_io {

   struct IOChar
   {
      char data = '\0';
      bool success = false;
   };

   class CharType
   {
     public:
      typedef int enum_base_type;
      enum Types : enum_base_type
      {
         DIGIT = 0,
         MINUS,
         PLUS,
         DOT,
         EQUAL,
         LESS,
         GREATER,
         COMMA,
         UNDERSCORE,
         ALPHA,
         UNKNOWN,
      };
      CharType() {}
      CharType(char data)
      {
         if (isdigit(data))
            types = DIGIT;
         else if (data == '-')
            types = MINUS;
         else if (data == '+')
            types = PLUS;
         else if (data == '.')
            types = DOT;
         else if (data == '=')
            types = EQUAL;
         else if (data == '<')
            types = LESS;
         else if (data == '>')
            types = GREATER;
         else if (data == ',')
            types = COMMA;
         // else if (data == '_')
         //    types = UNDERSCORE;
         // else if (isalpha(data))
         //    types = ALPHA;
      }
      bool isSignChar() const
      {
         return types == MINUS || types == PLUS;
      }
      bool isRecognized() const { return types != UNKNOWN; }
      bool operator==(Types t) const { return types == t; }
      bool operator==(enum_base_type t) const { return types == t; }

     private:
      Types types = UNKNOWN;
   };

   IOChar try_get(std::istream& in)
   {
      IOChar result;
      in.get(result.data);
      if (in.fail() || in.gcount() == 0)
         result.success = false;
      else
         result.success = true;
      return result;
   }

   std::string extractBufferContent(const std::string& buffer)
   {
      std::stringstream stream;
      for (auto&& i : buffer) {
         if (i != '\0')
            stream << i;
      }
      return stream.str();
   }

   enum ReturnAction
   {
      RA_SUCCESS,
      RA_ERROR_UNKNOWN_SYMBOL,
      RA_LOOP_CONTINUE,
      RA_LOOP_BREAK,
   };

   ReturnAction parseCoef(const IOChar& input, ParsedSymbolType& s,
                          size_t& digit_count, size_t& dot_count)
   {
      ReturnAction action = RA_SUCCESS;
      bool valid = true;
      CharType t = { input.data };

      if (s == TYPE_COEFF) {
         if (t == CharType::DIGIT)
            ++digit_count;
         else if (t == CharType::DOT) {
            if (dot_count == 0)
               ++dot_count;
            else
               valid = false;
         } else
            valid = false;
      } else {
         if (t == CharType::DIGIT) {
            s = TYPE_COEFF;
            ++digit_count;
         } else
            action = RA_ERROR_UNKNOWN_SYMBOL;
      }
      if (!valid)
         action = RA_LOOP_BREAK;
      return action;
   }
   ReturnAction parseConstraint(const IOChar& input,
                                ParsedSymbolType& s)
   {
      // Read last part of "<=" or ">="
      if (input.data == '=')
         return RA_SUCCESS;
      else
         return RA_ERROR_UNKNOWN_SYMBOL;
   }
   ReturnAction parseMinmax(const IOChar& input, ParsedSymbolType& s,
                            size_t& minmax_index)
   {
      ReturnAction action = RA_SUCCESS;
      bool valid = true;
      CharType t = { input.data };

      if (s & (TYPE_MIN | TYPE_MAX) &&
          (s & (~(TYPE_MIN | TYPE_MAX))) == 0) {
         if (s == (TYPE_MIN | TYPE_MAX)) {
            if (input.data == min_str[minmax_index] &&
                input.data == max_str[minmax_index])
               s == TYPE_MIN | TYPE_MAX;
            else if (input.data == min_str[minmax_index])
               s = TYPE_MIN;
            else if (input.data == max_str[minmax_index])
               s = TYPE_MAX;
            else
               valid = false;
            if (valid)
               ++minmax_index;
         } else if (s == TYPE_MIN &&
                    input.data == min_str[minmax_index])
            ++minmax_index;
         else if (input.data == max_str[minmax_index]) {
            ++minmax_index;
         } else
            valid = false;
      } else {
         if (t == CharType::GREATER) {
            s = static_cast<ParsedSymbolType>(TYPE_MIN | TYPE_MAX);
            // in next call we expect input.data = 'm', so set index =
            // 2
            minmax_index = 2;
         } else
            action = RA_ERROR_UNKNOWN_SYMBOL;
      }
      if (!valid)
         action = RA_LOOP_BREAK;
      return action;
   }
   void filterSpaces(std::string& str)
   {
      size_t size = str.size(), null_count = 0;
      for (size_t i = 0; i < size; ++i) {
         if (isspace(str[i])) {
            str.erase(i, 1);
            --i;
            --size;
         } else if (str[i] == '\0') {
            if (null_count == 0)
               ++null_count;
            else {
               str.resize(i - 1);
               break;
            }
         }
      }
   }
   ParsedSymbol construct(const ParsedSymbolType& s,
                          std::string& buffer)
   {
      filterSpaces(buffer);
      if (s == TYPE_COEFF) {
         return Coefficient(std::stod(buffer));
      } else if (s == TYPE_MIN || s == TYPE_MAX) {
         if (buffer == min_str)
            return MIN;
         else if (buffer == max_str)
            return MAX;
      } else if (s == TYPE_CONSTRAINT) {
         if (buffer == ">=")
            return GREATER_OR_EQUAL;
         else if (buffer == "<=")
            return LESSER_OR_EQUAL;
         else if (buffer == "=")
            return EQUAL;
      }
      return s;
   }
   ParsedSymbol next_symbol(std::iostream& in,
                            const size_t buffer_size = 255)
   {
      std::string buffer, tmp;
      buffer.resize(buffer_size);
      tmp.resize(buffer_size);

      ParsedSymbol result;
      ParsedSymbolType s = TYPE_NULL;
      bool valid = true, parsingCoefValue = false;
      ReturnAction action = RA_ERROR_UNKNOWN_SYMBOL;

      std::fill(buffer.begin(), buffer.end(), '\0');
      size_t minmax_index = 0, digit_count = 0, dot_count = 0;
      for (size_t i = 0; i < buffer_size; ++i) {
         bool isCharParsed = false;
         IOChar input = try_get(in);
         if (!input.success) {
            break;
         }
         if (input.data == '#') {
            s = TYPE_COMMENT;
            in.getline(tmp.data(), buffer_size);
            break;
         }

         if (isspace(input.data)) {
            isCharParsed = true;
            if (s == TYPE_COEFF && parsingCoefValue)
               break;
            if (s == TYPE_COMMA)
               break;
         }

         if (!isCharParsed && s == TYPE_NULL) {
            CharType t(input.data);
            if (t == CharType::DIGIT) {
               s = TYPE_COEFF;
               parsingCoefValue = true;
            }
            if (t == CharType::MINUS) {
               s = static_cast<ParsedSymbolType>(TYPE_COEFF |
                                                 TYPE_MIN | TYPE_MAX);
            } else if (t == CharType::PLUS) {
               s = TYPE_COEFF;
            } else if (t == CharType::GREATER ||
                       t == CharType::LESS || t == CharType::EQUAL) {
               s = TYPE_CONSTRAINT;
            } else if (t == CharType::COMMA) {
               s = TYPE_COMMA;
            }
            if (s != TYPE_NULL)
               isCharParsed = true;
         }
         if (!isCharParsed) {
            switch (s) {
               case TYPE_COEFF:
                  action =
                    parseCoef(input, s, digit_count, dot_count);
                  if (action == RA_SUCCESS) {
                     parsingCoefValue = true;
                  } else if (input.data == ',') {
                     in.unget();
                     action = RA_SUCCESS;
                  }
                  break;
               case TYPE_MIN | TYPE_MAX:
               case TYPE_MIN:
               case TYPE_MAX:
                  action = parseMinmax(input, s, minmax_index);
                  break;
               case TYPE_CONSTRAINT:
                  action = parseConstraint(input, s);
                  break;
               case TYPE_COEFF | TYPE_MIN | TYPE_MAX:
                  action =
                    parseCoef(input, s, digit_count, dot_count);
                  if (action == RA_ERROR_UNKNOWN_SYMBOL) {
                     action = parseMinmax(input, s, minmax_index);
                  } else if (action == RA_SUCCESS)
                     parsingCoefValue = true;
                  break;
               default:
                  break;
            }
            if (action != RA_SUCCESS)
               valid = false;
            if (action == RA_LOOP_BREAK)
               break;
         }
         if (s == TYPE_COEFF && input.data == ',')
            break;
         buffer[i] = input.data;
         if (s == TYPE_MIN && minmax_index >= min_str.size())
            break;
         else if (s == TYPE_MAX && minmax_index >= max_str.size())
            break;
         else if (s == TYPE_CONSTRAINT && input.data == '=')
            break;
         else if (s == TYPE_COMMA)
            break;
      }
      if (!valid) {
         std::string msg;
         buffer = extractBufferContent(buffer);
         msg = "'" + buffer + "',char index=" +
               std::to_string(in.tellg() % buffer.size()) +
               ". Syntax error.";

         throw std::runtime_error(msg);
      }
      result = construct(s, buffer);
      return result;
   }

   constexpr bool isCommentLine(std::string_view line)
   {
      auto pos = line.find_first_of('#');
      if (pos == line.npos)
         return false;
      else {
         for (size_t i = 0; i < pos; ++i) {
            if (!isspace(line[i]))
               return false;
         }
      }
      return true;
   }

   constexpr bool isEmptyLine(std::string_view line)
   {
      for (size_t i = 0; i < line.size(); ++i) {
         if (!isspace(line[i]))
            return false;
      }
      return true;
   }

   std::string next_line(std::istream& in, const size_t line_length)
   {
      std::string result;
      while (true) {
         result.resize(line_length);
         std::fill(result.begin(), result.end(), '\0');
         in.getline(result.data(), line_length, '\n');
         if (in.fail() || in.gcount() == 0)
            throw std::runtime_error(
              "Cannot read line, EOF reached or IO error happened");
         result = extractBufferContent(result);
         if (!isCommentLine(result) && !isEmptyLine(result))
            break;
      }

      return result;
   };

   ParsedFunction readFunction(std::istream& in,
                               const size_t buffer_size)
   {
      ParsedFunction result;
      std::stringstream buffer;
      constexpr size_t expected_label_length = 3;
      size_t var_index = 0;
      std::string label;
      label.reserve(expected_label_length);
      try {
         buffer.str(next_line(in, buffer_size));
      } catch (const std::exception& e) {
         result.success = false;
         return result;
      }
      do {
         ParsedSymbol s = next_symbol(buffer, buffer_size);
         switch (s.index()) {
            case 0:
               label = labelFromIndex(var_index);
               result.function.push_back(
                 { label, std::get<0>(s).val });
               ++var_index;
               break;
            case 1:
               result.function[var_index].first =
                 std::get<1>(s).label;
               break;
            case 2:
               result.function_type = std::get<2>(s);
               if (buffer)
                  buffer.setstate(std::ios::eofbit);
               break;
            case 4:
               if (std::get<4>(s) == TYPE_COMMENT) {
                  continue;
               } else {
                  if (buffer)
                     buffer.setstate(std::ios::eofbit);
               }
               break;
            default:
               break;
         }
      } while (buffer);

      return result;
   }

   ParsedConstraint readConstraint(std::istream& in,
                                   const size_t buffer_size)
   {
      ParsedConstraint result;
      std::stringstream buffer;
      constexpr size_t expected_label_length = 3;
      size_t var_index = 0, comma_count = 0;
      std::string label;
      label.reserve(expected_label_length);
      bool isTypeParsed = false, isParsingCompleted = false;
      double current_coef_val = 0;

      try {
         buffer.str(next_line(in, buffer_size));
      } catch (const std::exception& e) {
         result.success = false;
         return result;
      }
      do {
         size_t tmp_index;
         ParsedSymbol s = next_symbol(buffer, buffer_size);
         switch (s.index()) {
            case 0:
               current_coef_val = std::get<0>(s).val;
               result.constraint.push_back(current_coef_val);
               if (isTypeParsed)
                  isParsingCompleted = true;
               ++var_index;
               break;
            case 1:
               tmp_index = indexFromLabel(std::get<1>(s).label);
               if (tmp_index >= result.constraint.size())
                  result.constraint.resize(tmp_index);
               result.constraint[tmp_index] = current_coef_val;
               break;
            case 3:
               result.constraint_type = std::get<3>(s);
               isTypeParsed = true;
               break;
            case 4:
               if (std::get<4>(s) == TYPE_COMMENT) {
                  if (!isParsingCompleted) {
                     result.success = false;
                     buffer.setstate(std::ios::eofbit);
                  }
               }
               if (std::get<4>(s) == TYPE_COMMA) {
                  result.is_last = true;
                  ++comma_count;
               } else {
                  buffer.setstate(std::ios::eofbit);
               }
               break;
            default:
               break;
         }
         auto count_diff = static_cast<long>(var_index) -
                           static_cast<long>(comma_count);
         if (result.is_last && (count_diff != 1 && count_diff != 0)) {
            bool skip =
              (isTypeParsed && (s.index() == 3 || s.index() == 0));
            if (!skip) {
               result.success = false;
               buffer.setstate(std::ios::eofbit);
            }
         }
      } while (buffer);

      return result;
   }
} // namespace simplex_io
