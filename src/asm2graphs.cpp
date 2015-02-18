
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <locale>
#include <cassert>

void ltrim(std::string & s, char c) {
  size_t first = s.find_first_not_of(c);
  if (first != std::string::npos)
    s =  s.substr(first);
}

void rtrim(std::string & s, char c) {
  size_t last = s.find_last_not_of(c);
  s =  s.substr(0, last+1);
}

void trim(std::string & s, char c) {
  ltrim(s, c);
  rtrim(s, c);
}

enum inst_e {
  e_jmp,
  e_jz,
  e_jnz,
  e_jnb,
  e_retn,
  e_call,
  e_none
};

std::string instToString(inst_e inst) {
  switch (inst) {
    case e_jmp:  return "jmp";
    case e_jz:   return "jz";
    case e_jnz:  return "jnz";
    case e_jnb:  return "jnb";
    case e_retn: return "retn";
    case e_call: return "call";
    case e_none: return "";
    default: assert(false);
  }
}

bool isPrintable(char c) {
  return (c >= 32) && (c <= 126);
}

std::string sanitize(const std::string & label) {
  std::string res;
  for (int i = 0; i < label.size(); i++)
    if (isPrintable(label[i]))
      res += label[i];
  return res;
}

std::string label_to_tag(const std::string & label) {
  std::string tag(label);
  std::replace(tag.begin(), tag.end(), ':', '_');
  std::replace(tag.begin(), tag.end(), '$', '_');
  std::replace(tag.begin(), tag.end(), '?', '_');
  std::replace(tag.begin(), tag.end(), '+', '_');
  std::replace(tag.begin(), tag.end(), '-', '_');
  std::replace(tag.begin(), tag.end(), '.', '_');
  std::replace(tag.begin(), tag.end(), '@', '_');
  std::replace(tag.begin(), tag.end(), '[', '_');
  std::replace(tag.begin(), tag.end(), ']', '_');
  std::replace(tag.begin(), tag.end(), '*', '_');
  std::replace(tag.begin(), tag.end(), '<', '_');
  std::replace(tag.begin(), tag.end(), ' ', '_');
  return tag;
}

struct block_t;

struct routine_t {
  routine_t(const std::string & label_) : label(label_) {
//  std::cerr << " -> routine_t(\"" << label << "\")" << std::endl;
  }
  std::string label;
  std::vector<block_t *> blocks;

  void toJSON(std::ostream & out, std::string indent, bool inc_blk) const;
};



struct block_t {
  block_t(const std::string & label_) :
    label(label_),
    instructions(),
    callees(),
    out_true(""),
    out_false(""),
    last_inst(e_none)
  {
//  std::cerr << " -> block_t(\"" << label << "\")" << std::endl;
  }

  std::string label;
  std::vector<std::string> instructions;
  std::vector<std::string> callees;
  std::string out_true;
  std::string out_false;
  inst_e last_inst;

  void toJSON(std::ostream & out, std::string indent) const;
};
void toJSON(const std::string & filename, const std::vector<routine_t *> & routines);

bool ignore(const std::string & str) {
  return ( str[0] == '.' ||
           str[0] == '_' ||
           str.find("db") == 0 ||
           str.find("align") == 0 ||
           str.find("assume") == 0 ||
           str.find("include") == 0 ||
           str.find("unk_") == 0 ||
           str.find("extrn") == 0 ||
           str.find("dword_") == 0 ||
           str.find("lpMem") == 0 ||
           str.find("TimeZoneInformation") == 0 ||
           str.find("byte_") == 0 ||
           str.find("word_") == 0 ||
           str.find("= dword ptr") != std::string::npos
         );
}

bool isLabel(const std::string & str, std::string & label) {
  if (str[str.length() - 1] == ':') {
    label = str.substr(0, str.length() - 1);
//  std::cerr << "    Found label: " << label << std::endl;
    return true;
  }
  return false;
}

inst_e getInstruction(const std::string & str, std::string & target) {
  size_t pos = str.find_last_of(' ');
  if (pos != std::string::npos)
    target = str.substr(pos+1);

  if (str.find("jmp") == 0) {
    return e_jmp;
  }
  else if (str.find("jz") == 0) {
    return e_jz;
  }
  else if (str.find("jnz") == 0) {
    return e_jnz;
  }
  else if (str.find("jnb") == 0) {
    return e_jnb;
  }
  else if (str.find("retn") == 0) {
    return e_retn;
  }
  else if (str.find("call") == 0) {
    return e_call;
  }
  else return e_none;
}

bool isProcStart(const std::string & str, std::string & label) {
  if (str.find("proc near") != std::string::npos) {
    size_t first_space = str.find_first_of(' ');
    assert(first_space != std::string::npos);
    label = str.substr(0, first_space);
//  std::cerr << "    Found \"proc near\": " << label << std::endl;
    return true;
  }
  return false;
}

void nextBlock(
  block_t * & prev_blk, block_t * & curr_blk,
  routine_t * & curr_rtn,
  const std::string & lbl_blk, const std::string & lbl_rtn,
  std::vector<routine_t *> & routines
) {
  if (!lbl_rtn.empty()) {
    curr_rtn = new routine_t(lbl_rtn);
    routines.push_back(curr_rtn);
  }

  if (!lbl_blk.empty()) {
    if (curr_blk != NULL) {
      assert(prev_blk == NULL);
      curr_blk->out_true  = lbl_blk;
      curr_blk->out_false = "";
      curr_blk->last_inst = e_none;
    }
    else if (prev_blk != NULL) {
      prev_blk->out_false = lbl_blk;
      prev_blk = NULL;
    }

    curr_blk = new block_t(lbl_blk);
    curr_rtn->blocks.push_back(curr_blk);
  }
}

int main(int argc, char ** argv) {
//std::cerr << argv[1] << std::endl;

  std::ifstream input;
  input.open(argv[1]);
  assert(input.is_open());

  std::vector<routine_t *> routines;

  routine_t * curr_rtn = NULL;
  size_t cnt_rtn = 0;

  block_t   * prev_blk = NULL;
  block_t   * curr_blk = NULL;
  size_t cnt_blk = 0;

  std::string line;

  while (std::getline(input, line)) {
//  std::cerr << std::endl;
//  std::cerr << "Read: \"" << line << "\"" << std::endl;

    line = sanitize(line);

    size_t start_comment = line.find_first_of(';');
    if (start_comment != std::string::npos)
      line = line.substr(0, start_comment);

    if (!line.empty()) trim(line, ' ');

    if (!line.empty() && !ignore(line)) {

//    std::cerr << "Use : \"" << line << "\"" << std::endl;

      std::string label;
      if (isLabel(line, label)) {
        std::ostringstream oss_rtn;
        if (curr_rtn == NULL) oss_rtn << "rtn_" << cnt_rtn++;
        nextBlock(prev_blk, curr_blk, curr_rtn, label, oss_rtn.str(), routines);
      }
      else if (isProcStart(line, label)) {
        nextBlock(prev_blk, curr_blk, curr_rtn, label, label, routines);
      }
      else if (line.find("endp") != std::string::npos) {
        curr_rtn = NULL;
      }
      else {
        if (curr_blk == NULL && curr_rtn == NULL) {
          std::ostringstream oss_blk; oss_blk << "blk_" << cnt_blk++;
          std::ostringstream oss_rtn; oss_rtn << "rtn_" << cnt_rtn++;
          nextBlock(prev_blk, curr_blk, curr_rtn, oss_blk.str(), oss_rtn.str(), routines);
        }
        else if (curr_blk == NULL) {
          std::ostringstream oss_blk;
          oss_blk << "label_" << cnt_blk++;
          nextBlock(prev_blk, curr_blk, curr_rtn, oss_blk.str(), "", routines);
        }

        curr_blk->instructions.push_back(line);

        std::string target;
        curr_blk->last_inst = getInstruction(line, target);

        if (curr_blk->last_inst == e_call) { // Calls don't break the CFG
          curr_blk->callees.push_back(target);
          curr_blk->last_inst = e_none;
        }
        else if (curr_blk->last_inst != e_none) {
          if (curr_blk->last_inst != e_retn) {
            curr_blk->out_true = target;
            if (curr_blk->last_inst != e_jmp) // No false edge when non-conditional jump
              prev_blk = curr_blk;
          }
          curr_blk = NULL;
        }
      }
    }
  }

  input.close();

  toJSON(argv[1], routines);

  return 0;
}

void routine_t::toJSON(std::ostream & out, std::string indent, bool inc_blk) const {
  std::vector<block_t *>::const_iterator it_blk;
  std::vector<std::string>::const_iterator it_str;
  std::set<std::string>::const_iterator it_str_;
  std::vector<std::string> callees;
  for (it_blk = blocks.begin(); it_blk != blocks.end(); it_blk++)
    for (it_str = (*it_blk)->callees.begin(); it_str != (*it_blk)->callees.end(); it_str++)
      callees.push_back(*it_str);

  out << "{" << std::endl;
  out << indent << "  \"tag\":\"" << label_to_tag(label) << "\"," << std::endl;
  out << indent << "  \"label\":\"" << label << "\"," << std::endl;
  out << indent << "  \"type\":";
  if (label.find("sub_") == 0)
    out << "\"idapro\"," << std::endl;
  else if (label.find("rtn_") == 0)
    out << "\"ours\"," << std::endl;
  else
    out << "\"user\"," << std::endl;
  out << indent << "  \"callees\":[" << std::endl;
  for (it_str = callees.begin(); it_str != callees.end(); it_str++) {
    out << indent << "    { \"tag\":\"" << label_to_tag(*it_str) << "\" }";
    if (it_str != callees.end() - 1)
      out << ",";
    out << std::endl;
  }
  if (inc_blk) {
    out << indent << "  ]," << std::endl;
    out << indent << "  \"blocks\":[" << std::endl;
    out << indent << "    ";
    for (it_blk = blocks.begin(); it_blk != blocks.end(); it_blk++) {
      (*it_blk)->toJSON(out, indent + "    ");
      if (it_blk != blocks.end() - 1)
        out << ",";
    }
    out << indent << "  ]" << std::endl;
  }
  else
    out << indent << "  ]" << std::endl;
  out << indent << "}";
}

void toJSON(const std::vector<routine_t *> & routines, std::ostream & out, std::string indent, bool inc_blk) {
  std::vector<routine_t *>::const_iterator it_rtn;
  std::vector<routine_t *>::const_iterator it_rtn_;
  std::vector<block_t *>::const_iterator it_blk;
  std::vector<std::string>::const_iterator it_str;
  std::set<std::string>::const_iterator it_str_;

  std::set<std::string> rtn_seen;
  std::set<std::string> rtn_not_seen;
  for (it_rtn = routines.begin(); it_rtn != routines.end(); it_rtn++)
    rtn_seen.insert((*it_rtn)->label);
  out << indent << "{" << std::endl;
  out << indent << "  \"routines\": [" << std::endl;
  out << indent << "  ";
  for (it_rtn = routines.begin(); it_rtn != routines.end(); it_rtn++) {
    routine_t * routine = *it_rtn;

    routine->toJSON(out, indent + "  ", inc_blk);

    for (it_blk = routine->blocks.begin(); it_blk != routine->blocks.end(); it_blk++)
      for (it_str = (*it_blk)->callees.begin(); it_str != (*it_blk)->callees.end(); it_str++)
        if (rtn_seen.find(*it_str) == rtn_seen.end())
          rtn_not_seen.insert(*it_str);

    if (it_rtn != routines.end() - 1)
      out << indent << ",";
  }
  for (it_str_ = rtn_not_seen.begin(); it_str_ != rtn_not_seen.end(); it_str_++) {
    out << ",{" << std::endl;
    out << indent << "    \"tag\":\"" << label_to_tag(*it_str_) << "\"," << std::endl;
    out << indent << "    \"label\":\"" << *it_str_ << "\"," << std::endl;
    out << indent << "    \"type\":";
    if ((*it_str_)[0] == '[')
      out << indent << "\"indirect\"," << std::endl;
    else
      out << indent << "\"library\"," << std::endl;
    out << indent << "  \"callees\":[]" << std::endl;
    out << indent << "  }";
  }
  out << indent << std::endl;
  out << indent << "  ]" << std::endl;
  out << indent << "}";
}

void block_t::toJSON(std::ostream & out, std::string indent) const {
  std::vector<std::string>::const_iterator it_str;
  out << "{" << std::endl;
  out << indent << "  \"tag\":\"" << label_to_tag(label) << "\"," << std::endl;
  out << indent << "  \"label\":\"" << label << "\"," << std::endl;
  out << indent << "  \"out_true\":\""  << label_to_tag(out_true)  << "\"," << std::endl;
  out << indent << "  \"out_false\":\"" << label_to_tag(out_false) << "\"," << std::endl;
  out << indent << "  \"last_inst\":\"" << instToString(last_inst) << "\"," << std::endl;
  out << indent << "  \"instructions\":[" << std::endl;
  for (it_str = instructions.begin(); it_str != instructions.end(); it_str++) {
    out << indent << "    { \"str\":\"" << *it_str << "\" }";
    if (it_str != instructions.end() - 1)
      out << ",";
    out << std::endl;
  }
  out << indent << "  ]," << std::endl;
  out << indent << "  \"callees\":[" << std::endl;
  for (it_str = callees.begin(); it_str != callees.end(); it_str++) {
    out << indent << "    { \"tag\":\"" << label_to_tag(*it_str) << "\" }";
    if (it_str != callees.end() - 1)
      out << ",";
    out << std::endl;
  }
  out << indent << "  ]" << std::endl;
  out << indent << "}";
}

void toJSON(const std::vector<block_t *> & blocks, std::ostream & out, std::string indent) {
  std::vector<block_t *>::const_iterator it_blk;
  std::set<std::string> blk_seen;
  std::set<std::string> blk_not_seen;
  std::set<std::string>::const_iterator it_str;

  for (it_blk = blocks.begin(); it_blk != blocks.end(); it_blk++)
    blk_seen.insert((*it_blk)->label);

  out << indent << "{" << std::endl;
  out << indent << "  \"blocks\":[" << std::endl;
  out << indent << "    ";
  for (it_blk = blocks.begin(); it_blk != blocks.end(); it_blk++) {
    block_t * block = *it_blk;

    block->toJSON(out, indent + "    ");

    if (!block->out_true.empty() && blk_seen.find(block->out_true) == blk_seen.end())
      blk_not_seen.insert(block->out_true);

    if (!block->out_false.empty() && blk_seen.find(block->out_false) == blk_seen.end())
      blk_not_seen.insert(block->out_false);

    if (it_blk != blocks.end() - 1)
      out << indent << ",";
  }
  for (it_str = blk_not_seen.begin(); it_str != blk_not_seen.end(); it_str++) {
    out << ",{" << std::endl;
    out << indent << "      \"tag\":\"" << label_to_tag(*it_str) << "\"," << std::endl;
    out << indent << "      \"label\":\"" << *it_str << "\"," << std::endl;
    out << indent << "      \"out_true\":\"\"," << std::endl;
    out << indent << "      \"out_false\":\"\"," << std::endl;
    out << indent << "      \"last_inst\":\"\"," << std::endl;
    out << indent << "      \"instructions\":[]," << std::endl;
    out << indent << "      \"callees\":[]" << std::endl;
    out << indent << "    }";
  }
  out << indent << std::endl;
  out << indent << "  ]" << std::endl;
  out << indent << "}";
}

void toJSON(const std::string & filename, const std::vector<routine_t *> & routines) {
  std::vector<routine_t *>::const_iterator it_rtn;
  std::string indent("");
  std::ofstream out;

  size_t slash_pos = filename.find_last_of('/');
  size_t dot_pos   = filename.find_last_of('.');
  std::string basename = filename.substr(slash_pos + 1, dot_pos - slash_pos - 1);

  out.open((basename + ".json").c_str());
  assert(out.is_open());
  toJSON(routines, out, indent, true);
  out.close();

  out.open((basename + "-no-block.json").c_str());
  assert(out.is_open());
  toJSON(routines, out, indent, false);
  out.close();

  for (it_rtn = routines.begin(); it_rtn != routines.end(); it_rtn++) {
    out.open((basename + "-" + label_to_tag((*it_rtn)->label) + ".json").c_str());
    assert(out.is_open());
    toJSON((*it_rtn)->blocks, out, indent);
    out.close();
  }
}

/*
#undef  DUMP_GRAPHVIZ
#define DUMP_JSON

  std::vector<routine_t *>::const_iterator it_rtn;
  std::vector<routine_t *>::const_iterator it_rtn_;
  std::vector<block_t *>::const_iterator it_blk;
  std::vector<std::string>::const_iterator it_str;
  std::ofstream out;
#if defined(DUMP_GRAPHVIZ)
  out.open((basename + ".dot").c_str());
  assert(out.is_open());
  out << indent <<"digraph cg {" << std::endl;
  out << indent <<"  rankdir=\"LR\";" << std::endl;
  for (it_rtn = routines.begin(); it_rtn != routines.end(); it_rtn++)
    out << indent <<"  " << label_to_tag((*it_rtn)->label) << " [label=\"" << (*it_rtn)->label << "\"];" << std::endl;
  for (it_rtn = routines.begin(); it_rtn != routines.end(); it_rtn++) {
    for (it_blk = (*it_rtn)->blocks.begin(); it_blk != (*it_rtn)->blocks.end(); it_blk++) {
      for (it_str = (*it_blk)->callees.begin(); it_str != (*it_blk)->callees.end(); it_str++) {
        out << indent <<"  " << label_to_tag((*it_rtn)->label) << " -> " << label_to_tag(*it_str);
        if ((*it_rtn)->label < *it_str) out << indent <<"[constraint=false]";
        out << indent <<";" << std::endl;
      }
    }
  }
  out << indent <<"}" << std::endl;
  out.close();
  
  for (it_rtn = routines.begin(); it_rtn != routines.end(); it_rtn++) {
    out.open((basename + "-" + label_to_tag((*it_rtn)->label) + ".dot").c_str());
    out << indent <<"digraph cfg {" << std::endl;
    for (it_blk = (*it_rtn)->blocks.begin(); it_blk != (*it_rtn)->blocks.end(); it_blk++) {
      out << indent <<"  " << label_to_tag((*it_blk)->label) << " [label=\"";
      if ((*it_blk)->instructions.size() > 7) {
        for (it_str = (*it_blk)->instructions.begin(); it_str != (*it_blk)->instructions.begin() + 3; it_str++)
          out << indent <<*it_str << "\\n";
        out << indent <<"...\\n";
        for (it_str = (*it_blk)->instructions.end() - 3; it_str != (*it_blk)->instructions.end(); it_str++)
          out << indent <<*it_str << "\\n";
      }
      else {
        for (it_str = (*it_blk)->instructions.begin(); it_str != (*it_blk)->instructions.end(); it_str++)
          out << indent <<*it_str << "\\n";
      }
      out << indent <<"\", shape=box];" << std::endl;

      if (!(*it_blk)->out_true.empty())
        out << indent <<"  " << label_to_tag((*it_blk)->label) << " -> " << label_to_tag((*it_blk)->out_true)  << "[constraint=true,  color=green];" << std::endl;
      if (!(*it_blk)->out_false.empty())
        out << indent <<"  " << label_to_tag((*it_blk)->label) << " -> " << label_to_tag((*it_blk)->out_false) << "[constraint=false, color=red  ];" << std::endl;
      
      for (it_str = (*it_blk)->callees.begin(); it_str != (*it_blk)->callees.end(); it_str++) {
        out << indent <<"  " << label_to_tag((*it_blk)->label) << " -> rtn_" << label_to_tag(*it_str) << "[constraint=false];" << std::endl;
      }
      out << indent <<"subgraph cluster_routines {" << std::endl;
      for (it_rtn_ = routines.begin(); it_rtn_ != routines.end(); it_rtn_++) {
        out << indent <<"  rtn_" << label_to_tag((*it_rtn_)->label) << " [label=\"" << (*it_rtn_)->label << "\"];" << std::endl;
      }
      out << indent <<"}" << std::endl;
    }
    out << indent <<"}" << std::endl;
    out.close();
  }
*/

