
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

#ifndef JSON
# define JSON 1
#endif

#ifndef VIZ
# define VIZ 0
#endif

#ifndef HIST
# define HIST 0
#endif

//trim from first non char c to end
//used to trim white space off the left of a string
void ltrim(std::string & s, char c) {
  size_t first = s.find_first_not_of(c);
  if (first != std::string::npos)
    s =  s.substr(first);
}

//find last occurance of non char c and return beginning to there
//used to trim whitespace off of right side of string
void rtrim(std::string & s, char c) {
  size_t last = s.find_last_not_of(c);
  s =  s.substr(0, last+1);
}

//used to trim white space off of both sides of string
void trim(std::string & s, char c) {
  ltrim(s, c);
  rtrim(s, c);
}

enum inst_e {
  e_jmp,

  e_ja,
  e_jb,
  e_jbe,
  e_jnb,
  e_jg,
  e_jl,
  e_jz,
  e_jnz,
  e_jle,
  e_jge,
  e_jo,
  e_jno,
  e_jp,
  e_jnp,
  e_js,
  e_jns,
  e_jcxz,
  e_jecxz,

  e_iret,
  e_iretw,
  e_retfw,
  e_retnw,
  e_sysret,
  e_retn,
  e_retf,

  e_call,

  e_none

};

std::string instToString(inst_e inst) {
  switch (inst) {
    case e_jmp:  return "jmp";

    case e_ja:   return "ja";
    case e_jb:   return "jb";
    case e_jbe:  return "jbe";
    case e_jnb:  return "jnb";
    case e_jg:   return "jg";
    case e_jl:   return "jl";
    case e_jz:   return "jz";
    case e_jnz:  return "jnz";
    case e_jge:  return "jge";
    case e_jle:  return "jle";
    case e_jo:   return "jo";
    case e_jno:  return "jno";
    case e_jp:   return "jp";
    case e_jnp:  return "jnp";
    case e_js:   return "js";
    case e_jns:  return "jns";
    case e_jcxz:   return "jcxz";
    case e_jecxz:  return "jecxz";

    case e_retn: return "retn";
    case e_retf: return "retf";
    case e_iret: return "iret";
    case e_iretw: return "iretw";
    case e_retfw: return "retfw";
    case e_retnw: return "retnw";
    case e_sysret: return "sysret";

    case e_call: return "call";

    case e_none: return "";
    default: assert(false);
  }
}

inst_e getInstruction(const std::string & str, std::string & target) {
  size_t pos = str.find_last_of(' ');
  if (pos != std::string::npos)
    target = str.substr(pos+1);

  if      (str.find("jmp" ) == 0) return e_jmp;

  else if (str.find("ja"  ) == 0) return e_ja;
  else if (str.find("jg"  ) == 0) return e_jg;
  else if (str.find("jge" ) == 0) return e_jge;
  else if (str.find("jl"  ) == 0) return e_jl;
  else if (str.find("jle" ) == 0) return e_jle;
  else if (str.find("jb"  ) == 0) return e_jb;
  else if (str.find("jbe" ) == 0) return e_jbe;
  else if (str.find("jnb" ) == 0) return e_jnb;
  else if (str.find("jo"  ) == 0) return e_jo;
  else if (str.find("jno" ) == 0) return e_jno;
  else if (str.find("jp"  ) == 0) return e_jp;
  else if (str.find("jnp" ) == 0) return e_jnp;
  else if (str.find("js"  ) == 0) return e_js;
  else if (str.find("jns" ) == 0) return e_jns;
  else if (str.find("jz"  ) == 0) return e_jz;
  else if (str.find("jnz" ) == 0) return e_jnz;
  else if (str.find("jcxz"  ) == 0) return e_jcxz;
  else if (str.find("jecxz" ) == 0) return e_jecxz;

  else if (str.find("iret"  ) == 0) return e_iret;
  else if (str.find("iretw" ) == 0) return e_iretw;
  else if (str.find("retf"  ) == 0) return e_retf;
  else if (str.find("retfw" ) == 0) return e_retfw;
  else if (str.find("retn"  ) == 0) return e_retn;
  else if (str.find("retnw" ) == 0) return e_retnw;
  else if (str.find("sysret") == 0) return e_sysret;

  else if (str.find("call") == 0) return e_call;
//else if (str.find("syscall") == 0) return e_syscall;
//else if (str.find("vmmcall") == 0) return e_vmmcall;

  else return e_none;
}

//returns whether the char c is in the printable ascii range
bool isPrintable(char c) {
  return (c >= 32) && (c <= 126);
}

//returns a 'sanitized' string
//strips out unprintable characters
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
  routine_t(const std::string & label_) : label(label_), blocks(), inst_hist() {
//  std::cerr << " -> routine_t(\"" << label << "\")" << std::endl;
  }
  std::string label;
  std::vector<block_t *> blocks;
  std::map<std::string, size_t> inst_hist;

  void collectHistogram();

  void toJSON(std::ostream & out, std::string indent, bool inc_blk) const;
};



struct block_t {
  block_t(const std::string & label_) :
    label(label_),
    instructions(),
    inst_hist(),
    callees(),
    out_true(""),
    out_false(""),
    last_inst(e_none)
  {
//  std::cerr << " -> block_t(\"" << label << "\")" << std::endl;
  }

  std::string label;
  std::vector<std::string> instructions;
  std::map<std::string, size_t> inst_hist;
  std::set<std::string> callees;
  std::string out_true;
  std::string out_false;
  inst_e last_inst;

  void addInstruction(const std::string & inst) {
    instructions.push_back(inst);

    size_t pos = inst.find_first_of(' ');
    inst_hist[inst.substr(0, pos)]++;
  }

  void toJSON(std::ostream & out, std::string indent) const;
};

void toJSON(const std::string & filename, const std::vector<routine_t *> & routines);

void routine_t::collectHistogram() {
  std::vector<block_t *>::const_iterator it_blk;
  std::map<std::string, size_t>::const_iterator it_hist;
  for (it_blk = blocks.begin(); it_blk != blocks.end(); it_blk++)
    for (it_hist = (*it_blk)->inst_hist.begin(); it_hist != (*it_blk)->inst_hist.end(); it_hist++)
      inst_hist[it_hist->first] += it_hist->second;
}

bool ignore(const std::string & str) {
  return ( str.length() == 1 ||
           str[0] == '.' ||
           str[0] == '_' ||
           str[0] == '+' ||
           str[0] == '0' ||
           str[0] == '1' ||
           str[0] == '2' ||
           str[0] == '3' ||
           str[0] == '4' ||
           str[0] == '5' ||
           str[0] == '6' ||
           str[0] == '7' ||
           str[0] == '8' ||
           str[0] == '9' ||
           str[0] == 'A' ||
           str[0] == 'B' ||
           str[0] == 'C' ||
           str[0] == 'D' ||
           str[0] == 'E' ||
           str[0] == 'F' ||
           str.find("t.text:0041DF14") == 0 ||
           str.find("e.text:00422CE2") == 0 ||
           str.find("sect_1") == 0   ||
           str.find("qmoyiu") == 0   ||
           str.find("ptest") == 0    ||
           str.find("psignw") == 0   ||
           str.find("pi2fw") == 0    ||
           str.find("oukci") == 0    ||
           str.find("meikik") == 0   ||
           str.find("kwyekk") == 0   ||
           str.find("iuagwws") == 0  ||
           str.find("eqooku") == 0   ||
           str.find("ecasws") == 0   ||
           str.find("acggagg") == 0  ||
           str.find("hResInfo") == 0 ||
           str.find("CriticalSection") == 0 ||
           str.find("Translated") == 0 ||
           str.find("String") == 0 ||
           str.find("Point") == 0 ||
           str.find("db") == 0 ||
           str.find(" db ") != std::string::npos ||
           str.find("dd") == 0 ||
           str.find(" dd ") != std::string::npos ||
           str.find("dw") == 0 ||
           str.find(" dw ") != std::string::npos ||
           str.find("dt") == 0 ||
           str.find(" dt ") != std::string::npos ||
           str.find("<0>") != std::string::npos ||
           str.find("align") == 0 ||
           str.find("assume") == 0 ||
           str.find("include") == 0 ||
           str.find("unk_") == 0 ||
           str.find("stru_") == 0 ||
           str.find("extrn") == 0 ||
           str.find("var_") == 0 ||
           str.find("CreationTime") == 0 ||
           str.find("ExitTime") == 0 ||
           str.find("KernelTime") == 0 ||
           str.find("UserTime") == 0 ||
           str.find("end") == 0 ||
           str.find("ExceptionInfo") == 0 ||
           str.find("unicode") == 0 ||
           str.find("dword_") == 0 ||
           str.find("lpMem") == 0 ||
           str.find("TimeZoneInformation") == 0 ||
           str.find("byte_") == 0 ||
           str.find("word_") == 0 ||
           str.find("public") == 0 ||
           str.find("=") != std::string::npos ||
           str.find("?") == 0 ||
           str.find("xmmword_") == 0 ||
           str.find("arg_") == 0 ||
           str.find("messageMap_") == 0 ||
           str.find("qword_") == 0 ||
           str.find("pStubDescriptor") != std::string::npos ||
           str.find("pows_") == 0 ||
           str.find("ObjectAttributes") == 0 ||
           str.find("messageMap") == 0 ||
           str.find("GS_") == 0 ||
           str.find("EventAttributes") == 0 ||
           str.find("CODE") == 0 ||
           str.find("AppName") == 0 ||
           str.find("AFX_MSGMAP_ENTRY") == 0 ||
           str.find("afxDoublePseudoNull") == 0 ||
           str.find("phkResult") == 0 ||
           str.find("rgFuncEntry") == 0 ||
           str.find("rgStdEntries") == 0 ||
           str.find("rgStdEntriesDLL") == 0 ||
           str.find("SecurityAttributes") == 0 ||
           str.find("seg00") == 0 ||
           str.find("STANDARD_ENTRY") == 0 ||
           str.find("String1") == 0 ||
           str.find("ud2") == 0 ||
           str.find("UPX") == 0 ||
           str.find("VxDCall") == 0 ||
           str.find("VxDJmp") == 0 ||
           str.find("hModule") == 0 ||
           str.find("ValueName") == 0 ||
           str.find("_____u1") == 0 ||
           str.find("dfd") == 0 ||
           str.find("FontDesc") == 0
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

bool isProcStart(const std::string & str, std::string & label) {
  if (
    str.find("proc near") != std::string::npos ||
    str.find("proc far") != std::string::npos
  ) {
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

//main
int main(int argc, char ** argv) {
//std::cerr << argv[1] << std::endl;

  //file stream for input
  std::ifstream input;
  //open file passed as input for reading
  input.open(argv[1]);
  //really check if it's open
  assert(input.is_open());

  //dynamic array of the routine_t type defined above
  std::vector<routine_t *> routines;

  routine_t * curr_rtn = NULL;
  size_t cnt_rtn = 0;

  block_t   * prev_blk = NULL;
  block_t   * curr_blk = NULL;
  size_t cnt_blk = 0;
  
  //string to read the current line into
  std::string line;

  //while there are still lines to read
  while (std::getline(input, line)) {
//  std::cerr << std::endl;
//  std::cerr << "Read: \"" << line << "\"" << std::endl;
   
    //strip the unprintables
    line = sanitize(line);
   
    //find where an assembly comment starts
    size_t start_comment = line.find_first_of(';');
    //if there is a comment
    if (start_comment != std::string::npos)
      //only keep the instruction part
      line = line.substr(0, start_comment);
    //if line is instruction(?) then trim white space off either side
    if (line.length() > 1) trim(line, ' ');
    //if is instruction(?) and its not a string we should ignore
    if ((line.length() > 1) && !ignore(line)) {

//    std::cerr << "Use : \"" << line << "\"" << std::endl;

      //string for the label
      std::string label;
      //if it is a label
      if (isLabel(line, label)) {
        //string stream to operate on
        std::ostringstream oss_rtn;
        //if the current routine is empty then send "rtn_" + the number of routines
        //into the stream
        if (curr_rtn == NULL) oss_rtn << "rtn_" << cnt_rtn++;
        //
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

        curr_blk->addInstruction(line);

        std::string target;
        curr_blk->last_inst = getInstruction(line, target);

        if (curr_blk->last_inst == e_call) { // Calls don't break the CFG
          curr_blk->callees.insert(target);
          curr_blk->last_inst = e_none;
        }
        else if (curr_blk->last_inst != e_none) {
          if (
               curr_blk->last_inst != e_sysret &&
               curr_blk->last_inst != e_retn  &&
               curr_blk->last_inst != e_iret  &&
               curr_blk->last_inst != e_iretw &&
               curr_blk->last_inst != e_retfw &&
               curr_blk->last_inst != e_retnw &&
               curr_blk->last_inst != e_retf
          ) {
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

  std::string filename(argv[1]);

  toJSON(filename, routines);

#if HIST == 1
  std::map<std::string, size_t> inst_hist;
  std::map<std::string, size_t> inst_hist_rtn;

  std::map<std::string, size_t> blk_last_inst_hist;

  std::map<size_t, size_t> inst_per_blk_hist;
  std::map<size_t, size_t> inst_per_rtn_hist;
  std::map<size_t, size_t> blk_per_rtn_hist;

  std::map<size_t, size_t> dir_call_per_rtn_hist;
  std::map<size_t, size_t> ind_call_per_rtn_hist;
  std::map<size_t, size_t> lib_call_per_rtn_hist;

  std::map<size_t, size_t> dir_call_per_blk_hist;
  std::map<size_t, size_t> ind_call_per_blk_hist;
  std::map<size_t, size_t> lib_call_per_blk_hist;

  std::map<std::string, size_t>::const_iterator it_hist;
  std::map<size_t, size_t>::const_iterator it_hist_;
  std::vector<routine_t *>::const_iterator it_rtn;
  std::vector<block_t *>::const_iterator it_blk;
  std::set<std::string>::const_iterator it_callee;

  size_t rtn_cnt = 0;
  size_t blk_cnt = 0;
  size_t inst_cnt = 0;

  std::map<std::string, routine_t *> lbl_rtn_map;
  for (it_rtn = routines.begin(); it_rtn != routines.end(); it_rtn++)
    lbl_rtn_map[(*it_rtn)->label] = *it_rtn;

  for (it_rtn = routines.begin(); it_rtn != routines.end(); it_rtn++) {
    rtn_cnt++;

    (*it_rtn)->collectHistogram();

    // Instructions histogran: collect histogram from prodecures and routines
    for (it_hist = (*it_rtn)->inst_hist.begin(); it_hist != (*it_rtn)->inst_hist.end(); it_hist++)
      inst_hist[it_hist->first] += it_hist->second;

    // "Deadcode" instructions histogran: collect histogram from routines
    if ((*it_rtn)->label.find("rtn_") == 0)
      for (it_hist = (*it_rtn)->inst_hist.begin(); it_hist != (*it_rtn)->inst_hist.end(); it_hist++)
        inst_hist_rtn[it_hist->first] += it_hist->second;

    // Instructions per Block
    for (it_blk = (*it_rtn)->blocks.begin(); it_blk != (*it_rtn)->blocks.end(); it_blk++)
      inst_per_blk_hist[(*it_blk)->instructions.size()]++;

    // Block's last instructions histogram
    for (it_blk = (*it_rtn)->blocks.begin(); it_blk != (*it_rtn)->blocks.end(); it_blk++)
      if ((*it_blk)->last_inst != e_none)
        blk_last_inst_hist[instToString((*it_blk)->last_inst)]++;
      else
        blk_last_inst_hist["label"]++;

    // Instructions per Routine
    size_t rtn_inst_cnt = 0;
    for (it_blk = (*it_rtn)->blocks.begin(); it_blk != (*it_rtn)->blocks.end(); it_blk++)
      rtn_inst_cnt += (*it_blk)->instructions.size();
    inst_per_rtn_hist[rtn_inst_cnt]++;

    // Global instruction count
    inst_cnt += rtn_inst_cnt;

    // Blocks per Routine
    blk_per_rtn_hist[(*it_rtn)->blocks.size()]++;
    blk_cnt += (*it_rtn)->blocks.size();

    size_t cnt_dir_call_rtn = 0;
    size_t cnt_ind_call_rtn = 0;
    size_t cnt_lib_call_rtn = 0;
    for (it_blk = (*it_rtn)->blocks.begin(); it_blk != (*it_rtn)->blocks.end(); it_blk++) {
      size_t cnt_dir_call_blk = 0;
      size_t cnt_ind_call_blk = 0;
      size_t cnt_lib_call_blk = 0;
      for (it_callee = (*it_blk)->callees.begin(); it_callee != (*it_blk)->callees.end(); it_callee++) {
        if (it_callee->find("[") == 0)
          cnt_ind_call_blk++;
        else if (it_callee->find("sub_") == 0 || it_callee->find("loc_") == 0)
          cnt_dir_call_blk++;
        else
          cnt_lib_call_blk++;
      }
      cnt_dir_call_rtn += cnt_dir_call_blk;
      dir_call_per_blk_hist[cnt_dir_call_blk]++;
      cnt_ind_call_rtn += cnt_ind_call_blk;
      ind_call_per_blk_hist[cnt_ind_call_blk]++;
      cnt_lib_call_rtn += cnt_lib_call_blk;
      lib_call_per_blk_hist[cnt_lib_call_blk]++;
    }
    dir_call_per_rtn_hist[cnt_dir_call_rtn]++;
    ind_call_per_rtn_hist[cnt_ind_call_rtn]++;
    lib_call_per_rtn_hist[cnt_lib_call_rtn]++;
  }
  rtn_cnt = routines.size();

  std::ofstream out;
  size_t slash_pos = filename.find_last_of('/');
  size_t dot_pos   = filename.find_last_of('.');
  std::string basename = filename.substr(slash_pos + 1, dot_pos - slash_pos - 1);

  out.open((basename + "-inst.hist").c_str());
  assert(out.is_open());
  for (it_hist = inst_hist.begin(); it_hist != inst_hist.end(); it_hist++)
    out << it_hist->first << " " << it_hist->second << std::endl;
  out.close();

  out.open((basename + "-inst-rtn.hist").c_str());
  assert(out.is_open());
  for (it_hist = inst_hist_rtn.begin(); it_hist != inst_hist_rtn.end(); it_hist++)
    out << it_hist->first << " " << it_hist->second << std::endl;
  out.close();

  out.open((basename + "-blk-last-inst.hist").c_str());
  assert(out.is_open());
  for (it_hist = blk_last_inst_hist.begin(); it_hist != blk_last_inst_hist.end(); it_hist++)
    out << it_hist->first << " " << it_hist->second << std::endl;
  out.close();

  out.open((basename + "-inst-per-blk.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = inst_per_blk_hist.begin(); it_hist_ != inst_per_blk_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-inst-per-rtn.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = inst_per_rtn_hist.begin(); it_hist_ != inst_per_rtn_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-blk-per-rtn.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = blk_per_rtn_hist.begin(); it_hist_ != blk_per_rtn_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-dir-call-per-rtn.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = dir_call_per_rtn_hist.begin(); it_hist_ != dir_call_per_rtn_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-ind-call-per-rtn.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = ind_call_per_rtn_hist.begin(); it_hist_ != ind_call_per_rtn_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-lib-call-per-rtn.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = lib_call_per_rtn_hist.begin(); it_hist_ != lib_call_per_rtn_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-dir-call-per-blk.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = dir_call_per_blk_hist.begin(); it_hist_ != dir_call_per_blk_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-ind-call-per-blk.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = ind_call_per_blk_hist.begin(); it_hist_ != ind_call_per_blk_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-lib-call-per-blk.hist").c_str());
  assert(out.is_open());
  for (it_hist_ = lib_call_per_blk_hist.begin(); it_hist_ != lib_call_per_blk_hist.end(); it_hist_++)
    out << it_hist_->first << " " << it_hist_->second << std::endl;
  out.close();

  out.open((basename + "-stat.hist").c_str());
  assert(out.is_open());
  out << "rtn_cnt "  << rtn_cnt  << std::endl;
  out << "blk_cnt "  << blk_cnt  << std::endl;
  out << "inst_cnt " << inst_cnt << std::endl;
  out.close();
#endif
  return 0;
}

void routine_t::toJSON(std::ostream & out, std::string indent, bool inc_blk) const {
  std::vector<block_t *>::const_iterator it_blk;
  std::set<std::string>::const_iterator it_str;

  std::set<std::string> callees;
  for (it_blk = blocks.begin(); it_blk != blocks.end(); it_blk++)
    for (it_str = (*it_blk)->callees.begin(); it_str != (*it_blk)->callees.end(); it_str++)
      callees.insert(*it_str);

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
  out << indent << "  \"callees\":[";
  if (!callees.empty()) {
    it_str = callees.begin();
    out << std::endl;
    out << indent << "    { \"tag\":\"" << label_to_tag(*it_str) << "\" }";
    it_str++;
    for ( ; it_str != callees.end(); it_str++) {
      out << "," << std::endl;
      out << indent << "    { \"tag\":\"" << label_to_tag(*it_str) << "\" }";
    }
    out << std::endl;
    out << indent << "  ";
  }
  if (inc_blk) {
    out << "]," << std::endl;
    out << indent << "  \"blocks\":[" << std::endl;
    out << indent << "    ";
    for (it_blk = blocks.begin(); it_blk != blocks.end(); it_blk++) {
      (*it_blk)->toJSON(out, indent + "    ");
      if (it_blk != blocks.end() - 1)
        out << ",";
      out << std::endl;
    }
    out << indent << "  ]" << std::endl;
  }
  else
    out << "]" << std::endl;
  out << indent << "}";
}

void toJSON(const std::vector<routine_t *> & routines, std::ostream & out, std::string indent, bool inc_blk) {
  std::vector<routine_t *>::const_iterator it_rtn;
  std::vector<routine_t *>::const_iterator it_rtn_;
  std::vector<block_t *>::const_iterator it_blk;
  std::set<std::string>::const_iterator it_str;

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
  for (it_str = rtn_not_seen.begin(); it_str != rtn_not_seen.end(); it_str++) {
    out << ",{" << std::endl;
    out << indent << "    \"tag\":\"" << label_to_tag(*it_str) << "\"," << std::endl;
    out << indent << "    \"label\":\"" << *it_str << "\"," << std::endl;
    out << indent << "    \"type\":";
    if ((*it_str)[0] == '[')
      out << indent << "\"indirect\"," << std::endl;
    else
      out << indent << "\"library\"," << std::endl;
    out << indent << "    \"callees\":[]," << std::endl;
    out << indent << "    \"blocks\":[]" << std::endl;
    out << indent << "  }";
  }
  out << indent << std::endl;
  out << indent << "  ]" << std::endl;
  out << indent << "}";
}

void block_t::toJSON(std::ostream & out, std::string indent) const {
  std::vector<std::string>::const_iterator it_str;
  std::set<std::string>::const_iterator it_str_;
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
  out << indent << "  \"callees\":[";
  if (!callees.empty()) {
    it_str_ = callees.begin();
    out << std::endl;
    out << indent << "    { \"tag\":\"" << label_to_tag(*it_str_) << "\" }";
    it_str_++;
    for (; it_str_ != callees.end(); it_str_++) {
      out << "," << std::endl;
      out << indent << "    { \"tag\":\"" << label_to_tag(*it_str_) << "\" }";
    }
    out << std::endl;
    out << indent << "  ";
  }
  out << "]" << std::endl;
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
#if JSON == 1
  out.open((basename + ".json").c_str());
  assert(out.is_open());
  toJSON(routines, out, indent, true);
  out.close();
#endif
#if VIZ == 1
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
#endif
}

