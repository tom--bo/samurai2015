#include "samurai.hpp"

Samurai::Samurai(CommentedIStream& cs) {
  cs >> name >> progname >> invocation;
}
void Samurai::readScore(CommentedIStream &ss) {
  ss >> score;
}

SamuraiDB::SamuraiDB() {};
SamuraiDB::SamuraiDB(string samuraiDBname, string scoreDBname) {
  ifstream samuraiDBstream(samuraiDBname);
  if ((samuraiDBstream.rdstate() & ifstream::failbit) != 0) {
    throw ErrorReport("Failed to open " + samuraiDBname);
  }
  CommentedIStream cs(samuraiDBstream);
  cs >> numSamurai >> programDir;
  samuraiList = new Samurai[numSamurai];
  for (int id = 0; id != numSamurai; id++) {
    samuraiList[id] = Samurai(cs);
  }
  ifstream scoreDBstream(scoreDBname);
  if ((scoreDBstream.rdstate() & ifstream::failbit) != 0) {
    throw ErrorReport("Failed to open " + scoreDBname);
  }
  CommentedIStream ss(scoreDBstream);
  Samurai* sorted[numSamurai];
  for (int id = 0; id != numSamurai; id++) {
    samuraiList[id].readScore(ss);
    sorted[id] = &samuraiList[id];
  }
  sort(sorted, sorted+numSamurai,
       [](Samurai* x, Samurai* y)-> bool { return (x->score > y->score); });
  for (int rank = 0; rank != numSamurai; rank++) {
    int score = sorted[rank]->score;
    int r = rank;
    while (r > 0 && sorted[r-1]->score == score)
      r--;
    sorted[rank]->rank = r;
  }
}

ostream& operator<<(ostream& os, const SamuraiDB& db) {
  for (int id = 0; id != db.numSamurai; id++) {
    Samurai* s = &db.samuraiList[id];
    os << s->rank << ": (" << s->score << ") "
       << s->name << "@" 
       << s->progname << " (" << s->invocation << ")" << endl;
  }
  return os;
}
