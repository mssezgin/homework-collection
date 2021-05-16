#include "moviestore.h"


MovieStore::MovieStore( ) //implemented, do not change
{
}


// IMPLEMENT
void
MovieStore::insert(const Movie & movie) {

    Movie *p;
    SKey k(movie);

    this->primaryIndex.insert(movie.getID(), movie);
    p = &this->primaryIndex.find(movie.getID())->data;
    this->secondaryIndex.insert(k, p);
    this->ternaryIndex.insert(k, p);
}


// IMPLEMENT
void
MovieStore::remove(const std::string & id) {

    MSTP::Node *p = this->primaryIndex.find(id);

    if (p) {

        SKey k(p->data);

        this->primaryIndex.remove(id);
        this->secondaryIndex.remove(k);
        this->ternaryIndex.remove(k);
    }
}


// IMPLEMENT
void
MovieStore::remove(const std::string & title, const std::string & director) {

    SKey k(title, director);
    MSTS::Node *p = this->secondaryIndex.find(k);

    if (p) {

        this->primaryIndex.remove(p->data->getID());
        this->secondaryIndex.remove(k);
        this->ternaryIndex.remove(k);
    }
}


// IMPLEMENT
void
MovieStore::removeAllMoviesWithTitle(const std::string & title) {

    SKey lower(title, "a");
    SKey upper(title, "{");
    std::list<MSTT::Node *> moviesToRemove = this->ternaryIndex.find(lower, upper);
    std::list<MSTT::Node *>::iterator it = moviesToRemove.begin();

    for (; it != moviesToRemove.end(); it++) {

        SKey k(*(*it)->data);

        this->primaryIndex.remove((*it)->data->getID());
        this->secondaryIndex.remove(k);
        this->ternaryIndex.remove(k);
    }
}


// IMPLEMENT
void
MovieStore::makeAvailable(const std::string & id) {

    MSTP::Node *p = this->primaryIndex.find(id);

    if (p) {

        p->data.setAvailable();
    }
}


// IMPLEMENT
void
MovieStore::makeUnavailable(const std::string & title, const std::string & director) {

    MSTT::Node *p = this->ternaryIndex.find(SKey(title, director));

    if (p) {

        p->data->setUnavailable();
    }
}


// IMPLEMENT
void
MovieStore::updateCompany(const std::string & director, const std::string & Company) {

    SKey lower("a", director);
    SKey upper("z", director);
    std::list<MSTS::Node *> moviesToUpdate = this->secondaryIndex.find(lower, upper);
    std::list<MSTS::Node *>::iterator it = moviesToUpdate.begin();

    for (; it != moviesToUpdate.end(); it++) {

        Movie *p = (Movie *)(*it)->data;
        p->setCompany(Company);
    }
}


// IMPLEMENT
void
MovieStore::printMoviesWithID(const std::string & id1, const std::string & id2, unsigned short since) const {

    std::list<MSTP::Node *> moviesToPrint = this->primaryIndex.find(id1, id2);
    std::list<MSTP::Node *>::iterator it = moviesToPrint.begin();

    for (; it != moviesToPrint.end(); it++) {

        if ((*it)->data.getYear() >= since) {

            std::cout << (*it)->data << std::endl;
        }
    }
}


// IMPLEMENT
void
MovieStore::printMoviesOfDirector(const std::string & director, const std::string & first, const std::string & last) const {

    SKey lower(first, director);
    SKey upper(last, director);
    std::list<MSTS::Node *> moviesToPrint = this->secondaryIndex.find(lower, upper);
    std::list<MSTS::Node *>::iterator it = moviesToPrint.begin();

    for (; it != moviesToPrint.end(); it++) {

        std::cout << *(*it)->data << std::endl;
    }
}



void  // implemented, do not change
MovieStore::printPrimarySorted( ) const
{
  printPrimarySorted(primaryIndex.getRoot());
}


void  // implemented, do not change
MovieStore::printPrimarySorted(MSTP::Node * t) const
{
  if (t == NULL)
  {
    return;
  }

  printPrimarySorted(t->left);
  std::cout << t->data << std::endl;
  printPrimarySorted(t->right);
}


void  // implemented, do not change
MovieStore::printSecondarySorted( ) const
{
  printSecondarySorted(secondaryIndex.getRoot());
}


void  // implemented, do not change
MovieStore::printSecondarySorted(MSTS::Node * t) const
{
  if (t == NULL)
  {
    return;
  }

  printSecondarySorted(t->left);
  std::cout << *(t->data) << std::endl;
  printSecondarySorted(t->right);
}


void  // implemented, do not change
MovieStore::printTernarySorted( ) const
{
  printTernarySorted(ternaryIndex.getRoot());
}


void  // implemented, do not change
MovieStore::printTernarySorted(MSTT::Node * t) const
{
  if (t == NULL)
  {
    return;
  }

  printTernarySorted(t->left);
  std::cout << *(t->data) << std::endl;
  printTernarySorted(t->right);
}

