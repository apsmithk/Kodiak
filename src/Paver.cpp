#include "Paver.hpp"

using namespace kodiak;

void PavingStyle::push_titleColor(const std::string title, const std::string color) {
  title_.push_back(title);
  color_.push_back(color);
}

void PavingStyle::set_title(const nat i, const std::string title) {
  if (i < title_.size()) {
    std::ostringstream os;
    os << "Kodiak (set_title): title's index " << i << " doesn't exist";
    throw Growl(os.str());
  }
  title_[i] = title;
}

void PavingStyle::set_color(const nat i, const std::string color) {
  if (i < title_.size()) {
    std::ostringstream os;
    os << "Kodiak (set_color): color's index " << i << " doesn't exist";
    throw Growl(os.str());
  }
  color_[i] = color;
}

void PrePaving::clear_boxes() {
  boxes_.clear();
  type_ = -1;
}

void PrePaving::push_box(const nat i, Box &box) {
  type_ = i;
  Box b = box;
  if (i >= boxes_.size())
    boxes_.resize(i + 1);
  boxes_[i].push_back(b);
}

// Move box from current type to j
void PrePaving::move_box_to(const nat j) {
  //assert(type_ >= 0);
  nat i = type_;
  if (i != j) {
    push_box(j, boxes_[i][0]);
    boxes_[i].clear();
  }
}

void PrePaving::insert(const PrePaving &paving) {
  //assert(type_ == -1);
  if (ntypes() < paving.ntypes())
    boxes_.resize(paving.ntypes());
  for (nat i = 0; i < paving.ntypes(); i++) {
    for (nat k = 0; k < paving.size(i); ++k) {
      boxes_[i].push_back(paving.boxes(i)[k]);
    }
  }
}

void Paving::gnuplot(const std::string filename,
                     const std::string v1, const std::string v2,
                     const PavingStyle &style) {
  if (empty()) return;
  nat var1 = varbox_.var(v1);
  nat var2 = varbox_.var(v2);
  std::ostringstream os;
  if (var1 >= nvars() || var2 >= nvars()) {
    os << "Kodiak (gnuplot_2D): variable name \"" << (var1 >= nvars() ? v1 : v2)
       << "\" doesn't exist in named box";
    throw Growl(os.str());
  }
  os << filename << "_" << v1 << "_" << v2;
  std::ofstream f;
  nat width = 2 * Kodiak::precision();

  std::ostringstream osf;

  bool fst = true;
  for (nat i = 0; i < ntypes(); ++i) {
    if (boxes(i).size() > 0) {
      std::ostringstream osb;
      osb << os.str() << ".dat" << i;
      f.open(osb.str().c_str(), std::ofstream::out);
      f << "## File: " << osb.str() << ". " << style.title(i) << ": " << boxes(i).size() << std::endl;
      f << "## " << std::setw(width - 3) << v1 << std::setw(width) << v2 << std::endl;
      gnuplot_boxes(f, boxes(i), var1, var2, width);
      f.close();
      osf << (fst ? "" : ", ") << "'" << osb.str() << "' title ' " << style.title(i)
          << " (" << boxes(i).size() << ") '"
          << (style.filled() ? " with filledcurves closed " : "")
          << "lc rgb '" << style.color(i) << "'";
      fst = false;
    }
  }

  std::ostringstream os2;
  os2 << os.str() << ".gplot";
  std::ofstream f2;
  f2.open(os2.str().c_str(), std::ofstream::out);
  f2 << "set style data lines" << std::endl;
  f2 << "set title \"Kodiak: (" << v1 << "," << v2
     << ") projection of " << filename << "'s paving\"" << std::endl;
  f2 << "set xlabel \"" << varbox_.names()[var1] << "\"" << std::endl;
  f2 << "set ylabel \"" << varbox_.names()[var2] << "\"" << std::endl;
  //if (terminal != "") {
  //  f2 << "set terminal " << terminal << std::endl;
  //  f2 << "set output '" << os.str() << "." << terminal << "'" << std::endl;
  //} else
  //  f2 << "set terminal x11" << std::endl;
  f2 << "plot ";
  if (!style.zoom()) {
    f2 << "[" << varbox_.box()[var1].inf() << ":" << varbox_.box()[var1].sup() << "] ";
    f2 << "[" << varbox_.box()[var2].inf() << ":" << varbox_.box()[var2].sup() << "] ";
  }
  f2 << osf.str() << std::endl;
  //if (terminal != "") {
  //  f2 << "print 'File " << os.str() << "." << terminal
  //     << " has been generated'" << std::endl;
  //} else
  //  f2 << "pause -1" << std::endl;
  f2.close();
  std::cout << "Kodiak (gnuplot_2D): Gnuplot script " << os2.str()
            << " and data files have been generated" << std::endl;
}

void Paving::gnuplot(const std::string filename,
                     const std::string v1, const std::string v2, const std::string v3,
                     const PavingStyle &style) {
  if (empty()) return;
  nat var1 = varbox_.var(v1);
  nat var2 = varbox_.var(v2);
  nat var3 = varbox_.var(v3);
  if (var1 >= nvars() || var2 >= nvars() || var3 >= nvars()) {
    std::ostringstream os;
    os << "Kodiak (gnuplot_3D): variable name \""
       << (var1 >= nvars() ? v1 : (var2 >= nvars() ? v2 : v3))
       << "\" doesn't exist in named box";
    throw Growl(os.str());
  }
  std::ostringstream os;
  os << filename << "_" << v1 << "_" << v2 << "_" << v3;
  std::ofstream f;
  nat width = 2 * Kodiak::precision();

  std::ostringstream osf;

  bool fst = true;
  for (nat i = 0; i < ntypes(); ++i) {
    if (boxes(i).size() > 0) {
      std::ostringstream osb;
      osb << os.str() << ".dat" << i;
      f.open(osb.str().c_str(), std::ofstream::out);
      f << "## File: " << osb.str() << ". " << style.title(i) << ": " << boxes(i).size() << std::endl;
      f << "## " << std::setw(width - 3) << v1 << std::setw(width) << v2
        << std::setw(width) << v3 << std::endl;
      gnuplot_boxes(f, boxes(i), var1, var2, var3, width);
      f.close();
      osf << (fst ? "" : ", ") << "'" << osb.str() << "' title ' " << style.title(i)
          << " (" << boxes(i).size() << ") '"
          << "lc rgb '" << style.color(i) << "'";
      fst = false;
    }
  }

  std::ostringstream os2;
  os2 << os.str() << ".gplot";
  std::ofstream f2;
  f2.open(os2.str().c_str(), std::ofstream::out);
  f2 << "set style data lines" << std::endl;
  f2 << "set title \"Kodiak: (" << v1 << "," << v2 << "," << v3
     << ") projection of " << filename << "'s paving\"" << std::endl;
  f2 << "set xlabel \"" << varbox_.names()[var1] << "\"" << std::endl;
  f2 << "set ylabel \"" << varbox_.names()[var2] << "\"" << std::endl;
  f2 << "set zlabel \"" << varbox_.names()[var3] << "\"" << std::endl;
  //if (terminal != "") {
  //  f2 << "set terminal " << terminal << std::endl;
  //  f2 << "set output '" << os.str() << "." << terminal << "'" << std::endl;
  //} else
  //  f2 << "set terminal x11" << std::endl;
  f2 << "splot ";
  if (!style.zoom()) {
    f2 << "[" << varbox_.box()[var1].inf() << ":" << varbox_.box()[var1].sup() << "] ";
    f2 << "[" << varbox_.box()[var2].inf() << ":" << varbox_.box()[var2].sup() << "] ";
    f2 << "[" << varbox_.box()[var3].inf() << ":" << varbox_.box()[var3].sup() << "] ";
  }
  f2 << osf.str() << std::endl;
  //if (terminal != "") {
  //  f2 << "print 'File " << os.str() << "." << terminal
  //     << " has been generated'" << std::endl;
  //} else
  //  f2 << "pause -1" << std::endl;
  f2.close();
  std::cout << "Kodiak (gnuplot_3D): Gnuplot script " << os2.str()
            << " and data files have been generated" << std::endl;
}

void Paving::gnuplot_2D(const std::string filename,
                        const std::string v1, const std::string v2,
                        const Names &titles, const Names &colors,
                        const bool zoom) const {
  if (empty()) return;
  nat var1 = varbox_.var(v1);
  nat var2 = varbox_.var(v2);
  std::ostringstream os;
  if (var1 >= nvars() || var2 >= nvars()) {
    os << "Kodiak (gnuplot_2D): variable name \"" << (var1 >= nvars() ? v1 : v2)
       << "\" doesn't exist in named box";
    throw Growl(os.str());
  }
  os << filename << "_" << v1 << "_" << v2;
  std::ofstream f;
  nat width = 2 * Kodiak::precision();

  std::ostringstream osf;

  bool fst = true;
  for (nat i = 0; i < ntypes(); ++i) {
    if (boxes(i).size() > 0) {
      std::ostringstream osb;
      osb << os.str() << ".dat" << i;
      f.open(osb.str().c_str(), std::ofstream::out);
      f << "## File: " << osb.str() << ". " << titles[i] << ": " << boxes(i).size() << std::endl;
      f << "## " << std::setw(width - 3) << v1 << std::setw(width) << v2 << std::endl;
      gnuplot_boxes(f, boxes(i), var1, var2, width);
      f.close();
      osf << (fst ? "" : ", ") << "'" << osb.str() << "' title '" << titles[i]
          << " (" << boxes(i).size() << ") '"
          << " with filledcurves closed "
          << "lc rgb '" << colors[i] << "'";
      fst = false;
    }
  }

  std::ostringstream os2;
  os2 << os.str() << ".gplot";
  std::ofstream f2;
  f2.open(os2.str().c_str(), std::ofstream::out);
  f2 << "set style data lines" << std::endl;
  f2 << "set title \"Kodiak: (" << v1 << "," << v2
     << ") projection of " << filename << "'s paving\"" << std::endl;
  f2 << "set xlabel \"" << varbox_.names()[var1] << "\"" << std::endl;
  f2 << "set ylabel \"" << varbox_.names()[var2] << "\"" << std::endl;
  //if (terminal != "") {
  //  f2 << "set terminal " << terminal << std::endl;
  //  f2 << "set output '" << os.str() << "." << terminal << "'" << std::endl;
  //} else
  //  f2 << "set terminal x11" << std::endl;
  f2 << "plot ";
  if (!zoom) {
    f2 << "[" << varbox_.box()[var1].inf() << ":" << varbox_.box()[var1].sup() << "] ";
    f2 << "[" << varbox_.box()[var2].inf() << ":" << varbox_.box()[var2].sup() << "] ";
  }
  f2 << osf.str() << std::endl;
  //if (terminal != "") {
  //  f2 << "print 'File " << os.str() << "." << terminal
  //     << " has been generated'" << std::endl;
  //} else
  //  f2 << "pause -1" << std::endl;
  f2.close();
  std::cout << "Kodiak (gnuplot_2D): Gnuplot script " << os2.str()
            << " and data files have been generated" << std::endl;
}

void Paving::gnuplot_3D(const std::string filename,
                        const std::string v1, const std::string v2, const std::string v3,
                        const Names &titles, const Names &colors,
                        const bool zoom) const {
  if (empty()) return;
  nat var1 = varbox_.var(v1);
  nat var2 = varbox_.var(v2);
  nat var3 = varbox_.var(v3);
  if (var1 >= nvars() || var2 >= nvars() || var3 >= nvars()) {
    std::ostringstream os;
    os << "Kodiak (gnuplot_3D): variable name \""
       << (var1 >= nvars() ? v1 : (var2 >= nvars() ? v2 : v3))
       << "\" doesn't exist in named box";
    throw Growl(os.str());
  }
  std::ostringstream os;
  os << filename << "_" << v1 << "_" << v2 << "_" << v3;
  std::ofstream f;
  nat width = 2 * Kodiak::precision();

  std::ostringstream osf;

  bool fst = true;
  for (nat i = 0; i < ntypes(); ++i) {
    if (boxes(i).size() > 0) {
      std::ostringstream osb;
      osb << os.str() << ".dat" << i;
      f.open(osb.str().c_str(), std::ofstream::out);
      f << "## File: " << osb.str() << ". " << titles[i] << ": " << boxes(i).size() << std::endl;
      f << "## " << std::setw(width - 3) << v1 << std::setw(width) << v2
        << std::setw(width) << v3 << std::endl;
      gnuplot_boxes(f, boxes(i), var1, var2, var3, width);
      f.close();
      osf << (fst ? "" : ", ") << "'" << osb.str() << "' title '" << titles[i]
          << " (" << boxes(i).size() << ") '"
          << "lc rgb '" << colors[i] << "'";
      fst = false;
    }
  }

  std::ostringstream os2;
  os2 << os.str() << ".gplot";
  std::ofstream f2;
  f2.open(os2.str().c_str(), std::ofstream::out);
  f2 << "set style data lines" << std::endl;
  f2 << "set title \"Kodiak: (" << v1 << "," << v2 << "," << v3
     << ") projection of " << filename << "'s paving\"" << std::endl;
  f2 << "set xlabel \"" << varbox_.names()[var1] << "\"" << std::endl;
  f2 << "set ylabel \"" << varbox_.names()[var2] << "\"" << std::endl;
  f2 << "set zlabel \"" << varbox_.names()[var3] << "\"" << std::endl;
  //if (terminal != "") {
  //  f2 << "set terminal " << terminal << std::endl;
  //  f2 << "set output '" << os.str() << "." << terminal << "'" << std::endl;
  //} else
  //  f2 << "set terminal x11" << std::endl;
  f2 << "splot ";
  if (!zoom) {
    f2 << "[" << varbox_.box()[var1].inf() << ":" << varbox_.box()[var1].sup() << "] ";
    f2 << "[" << varbox_.box()[var2].inf() << ":" << varbox_.box()[var2].sup() << "] ";
    f2 << "[" << varbox_.box()[var3].inf() << ":" << varbox_.box()[var3].sup() << "] ";
  }
  f2 << osf.str() << std::endl;
  //if (terminal != "") {
  //  f2 << "print 'File " << os.str() << "." << terminal
  //     << " has been generated'" << std::endl;
  //} else
  //  f2 << "pause -1" << std::endl;
  f2.close();
  std::cout << "Kodiak (gnuplot_3D): Gnuplot script " << os2.str()
            << " and data files have been generated" << std::endl;
}

void Paving::save(const std::string filename, const Names &titles, const Names &names) const {
  if (empty()) return;
  Tuple vs;
  for (nat v = 0; v < names.size(); ++v) {
    nat n = varbox_.var(names[v]);
    if (n < nvars())
      vs.push_back(n);
  }
  std::ostringstream os;
  os << filename;
  for (nat i = 0; i < vs.size(); ++i)
    os << "_" << varbox_.name(vs[i]);
  os << ".dat";
  if (vs.empty()) {
    vs.resize(varbox_.size());
    for (nat v = 0; v < varbox_.size(); ++v)
      vs[v] = v;
  }
  std::ofstream f;
  f.open(os.str().c_str(), std::ofstream::out);
  f << "## File: " << os.str() << std::endl;
  f << "## Type: " << type_ << std::endl;
  f << "## Vars:" << std::endl;
  nat width = 2 * Kodiak::precision();
  for (nat i = 0; i < vs.size(); ++i)
    f << std::setw(width) << varbox_.name(vs[i]);
  f << std::endl;
  for (nat i = 0; i < vs.size(); ++i)
    f << std::setw(width) << varbox_.box()[vs[i]].inf();
  f << std::endl;
  for (nat i = 0; i < vs.size(); ++i)
    f << std::setw(width) << varbox_.box()[vs[i]].sup();
  f << std::endl;
  f << std::endl;
  for (nat i = 0; i < titles.size(); ++i) {
    f << "## " << titles[i] << ": " << size(i) << " boxes " << std::endl;
    if (i < boxes_.size() && boxes_[i].size() > 0)
      save_boxes(f, boxes_[i], vs, width);
    else
      f << std::endl;
  }
  f.close();
  std::cout << "Kodiak (save): Boxes were saved in file " << os.str() << std::endl;
}

void Paving::write(const std::string filename) {
  std::ostringstream os;
  os << filename << ".pav";
  std::ofstream ofs(os.str().c_str());
  boost::archive::text_oarchive oa(ofs);
  const Paving pav = (*this);
  oa << pav;
}

void Paving::read(const std::string filename) {
  std::ostringstream os;
  os << filename << ".pav";
  std::ifstream ifs(os.str().c_str());
  boost::archive::text_iarchive ia(ifs);
  ia >> (*this);
}

// Compute the projection of a paving onto a smaller-dimensional space
// (restricted list of variables). After eliminating the redundant
// variables/intervals from all boxes, those boxes which are subsets
// of any already in the result paving are excluded.

Paving Paving::projection(const Names &names) {

  Paving result;
  NamedBox varbox;   // varbox for result, varbox_ for this paving
  Tuple varlist;   // list of variables to be retained
  varlist.resize(names.size());
  result.set_type(type_);

  // push the relevant names and intervals
  for (nat i = 0; i < names.size(); ++i) {
    nat v = varbox_.var(names[i]);
    if (v < varbox_.size()) {
      varbox.push(names[i], varbox_.val(v));
      varlist[i] = v;
    } else {
      std::ostringstream os;
      os << "Kodiak (projection): name \"" << names[i] << "\" doesn't exist in the paving.";
      throw Growl(os.str());
    }
  }
  result.set_varbox(varbox);

  // add boxes
  Box x;
  x.resize(names.size());
  if (boxes_.size() == 0) return result;
  for (nat i = 0; i < boxes_.size(); ++i) { // iterate over types
    if (boxes_[i].size() == 0) continue;
    // add the first box
    for (nat k = 0; k < names.size(); ++k) x[k] = boxes_[i][0][varlist[k]];
    result.push_box(i, x);
    if (boxes_[i].size() == 1) continue;
    // add remaining boxes where necessary
    for (nat j = 1; j < boxes_[i].size(); ++j) {
      for (nat k = 0; k < names.size(); ++k) x[k] = boxes_[i][j][varlist[k]];
      nat subset = 0;   // is the jth box a subset of any box already
      // pushed into the result paving?
      for (nat jj = 0; jj < result.boxes(i).size(); ++jj) {
        if (box_subset(result.boxes(i)[jj], x)) {
          subset = 1;
          break;
        }
      }
      if (subset == 0) result.push_box(i, x);
    }
  }

  for (nat i = 0; i < boxes_.size(); ++i) { // iterate over types
    encluster(result.boxes(i));
  }

  result.set_type(type_);
  return result;

}

PavingStyle Paver::default_style(const bool zoom, const bool filled) {
  PavingStyle pav = PavingStyle(zoom, filled);
  pav.push_titleColor("Certainly", "green");
  pav.push_titleColor("Possibly", "orange");
  pav.push_titleColor("Almost Certainly", "blue");
  pav.push_titleColor("Certainly Not", "red");
  return pav;
}

Paver::Paver(const std::string id) : flags_(0) {
  if (id == "") {
    std::ostringstream os;
    os << "P" << IDs_++;
    id_ = os.str();
  } else
    id_ = id;
  style = default_style();
}

void Paver::pave(const SearchType search) {
  temp_.resize(nvars());
  Ints certainties;
  certainties.assign(nrels() + flags_, -1);
  search_ = search;
  cert_ = 0;
  paving_.clear_boxes();
  bandb(paving_, certainties, varbox_.box());
  paving_.set_varbox(varbox_);
  paving_.set_type(search_);
}

void Paver::evaluate(PrePaving &paving, Ints &certainties, Box &box) {
  //std::cout << "Box: " << box << std::endl;
  //std::cout << "DirVars: " << dirvars() << std::endl;
  cert_ = 1;
  for (nat f = 0; f < nrels(); ++f) {
    // if f-th formula is certainly true, don't check it again
    if (certainties[f] > 0) continue;
    if (!dirvars().empty() && rels_[f].ope().isFresh(dirvars().back().var) &&
        where(dirvars(), dirvars().back().var) != EXTERIOR) {
      // top variable doesn't appear in f-th formula, don't check it again
      cert_ = (cert_ > 0 ? certainties[f] : std::max(cert_, certainties[f]));
      continue;
    }
    evalDefs();
    certainties[f] = rels_[f].eval(box, defbox_, bps_[f], tolerance_);
    // if f-th formula is certainly not true, conjunction of formulas is certainly not true
    if (certainties[f] == 0) {
      cert_ = 0;
      if (search_ == FULL)
        paving.push_box(3, box); // Whole interval (certainly not)
      return;
    }
    if (certainties[f] < 0)
      cert_ = (cert_ > 0 ? certainties[f] : std::max(cert_, certainties[f]));
  }
  if (cert_ > 0) // Conjunction of formulas is certainly true
    paving.push_box(0, box); // Whole interval (certainly)
  else // Conjunction of formulas is possibly true
    paving.push_box(std::abs(cert_), box); // Whole interval (possibly)
}

void Paver::sos_eval(Interval &X, Ints &certainties, const Box &box, const nat v) {
  evalDefs();
  X = Interval(0);
  for (nat f = 0; f < nrels(); ++f) {
    if (certainties[f] < 0 && rels_[f].ope().hasVar(v))
      X += sqr(rels_[f].ope().eval(box, defbox_));
  }
}

void Paver::select(DirVar &dirvar, Ints &certainties, Box &box) {
  for (nat v = 0; v < box.size(); ++v) {
    real mid = box[v].mid();
    temp_[v] = Interval(mid, mid);
  }
  dirvar.init(box.size());
  real max_diff;
  Interval d_it;
  for (nat v = 0; v < box.size(); ++v) {
    if (!box[v].isPoint() && (search_ == FIRST || box[v].diam() > std::max(resolutions_[v],
                              theBox()[v].diam()*resolutions_[v]))) {
      Interval save_mid = temp_[v];
      temp_[v] = box[v];
      sos_eval(d_it, certainties, temp_, v);
      real max_diff_it = d_it.diam();
      if ((dirvar.var == box.size() && max_diff_it != 0) || max_diff_it > max_diff) {
        dirvar.var = v;
        max_diff = max_diff_it;
      }
      temp_[v] = save_mid;
    }
  }
}
void Paver::combine(PrePaving &paving, const DirVar &,
                    const PrePaving &paving1) {
  paving.clear_boxes();
  if (!paving1.empty())
    paving.insert(paving1);
}

void Paver::combine(PrePaving &paving, const DirVar &,
                    const PrePaving &paving1, const PrePaving &paving2) {
  // Paving type can only be POSSIBLE and contain only one box
  if (paving1.type() == paving2.type() &&
      paving1.type() >= 0)
    paving.move_box_to(paving1.type());
  else {
    paving.clear_boxes();
    if (!paving1.empty())
      paving.insert(paving1);
    if (!paving2.empty())
      paving.insert(paving2);
  }
}

void Paver::print(std::ostream &os) const {
  os << "System: " << id_ << std::endl;
  os << "Vars: ";
  varbox_.print(os);
  os << std::endl;
  if (!constbox_.empty()) {
    os << "Consts: ";
    constbox_.print(os);
    os << std::endl;
  }
  if (!rels_.empty()) {
    os << "Constraints: " << std::endl;
    for (nat f = 0; f < nrels(); ++f)
      os << id_ << "_" << f << ": " << rels_[f] << std::endl;
  }
  if (debug() && nrels() > 0) {
    os << std::endl << "<BEGIN DEBUG INFORMATION>" << std::endl;
    os << "*** Partial derivatives: " << std::endl;
    for (nat f = 0; f < nrels(); ++f) {
      for (nat v = 0; v < nvars(); ++v)
        os << "d" << id_ << "_" << f << "/d" << varbox_.name(v)
           << " = " << rels_[f].d(v) << std::endl;
    }
    os << "<END DEBUG INFORMATION>" << std::endl << std::endl;
  }
  for (nat i = 0; i < paving_.ntypes(); i++)
    os << style.title(i) << ": " << paving_.size(i) << " boxes" << std::endl;
  print_info(os);
  os << std::endl;
  paving_.save(id_, style.titles());
}

