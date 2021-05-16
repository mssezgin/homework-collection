#include "SocialNetwork.h"

#include <iostream>

SocialNetwork::SocialNetwork() {

}

void SocialNetwork::addProfile(const std::string &firstname, const std::string &lastname, const std::string &email) {
    
    Profile newProfile(firstname, lastname, email);
    
    this->profiles.insertSorted(newProfile);
    //std::cout << "===returning from addProfile(" << firstname << ", " << lastname << ", " << email << ")\n";
}

void SocialNetwork::addPost(const std::string &message, const std::string &email) {
    
    Post newPost(message);
    Node<Profile> *owner = this->getNodeProfileWithEmail(email);
    
    this->posts.insertAtTheTail(newPost);
    owner->data.addPost(&this->posts.getLastNode()->data);
    //std::cout << "===returning from addPost(" << message << ", " << email << ")\n";
}

void SocialNetwork::deleteProfile(const std::string &email) {
    
    Node<Profile> *oldProfile = this->getNodeProfileWithEmail(email);
    Node<Profile> *others;
    Node<Profile *> *tmpFr;
    Node<Post *> *tmpPs;
    
    // deleting the user from the friends lists of his/her friends
    for (tmpFr = oldProfile->data.getFriends().getFirstNode(); tmpFr; tmpFr = tmpFr->next) {
        tmpFr->data->dropFriend(&oldProfile->data);
    }
    
    // deleting the user's posts from the likes lists of others
    for (tmpPs = oldProfile->data.getPosts().getFirstNode(); tmpPs; tmpPs = tmpPs->next) {
        for (others = this->profiles.getFirstNode(); others; others = others->next) {
            others->data.dropLike(tmpPs->data);
        }
        this->posts.removeNode(*tmpPs->data);
    }
    
    // deleting the user's contents and itself
    oldProfile->data.getFriends().removeAllNodes();
    oldProfile->data.getLikes().removeAllNodes();
    oldProfile->data.getPosts().removeAllNodes();
    this->profiles.removeNode(oldProfile);
    //std::cout << "===returning from deleteProfile(" << email << ")\n";
}

void SocialNetwork::makeFriends(const std::string &email1, const std::string &email2) {
    
    Node<Profile> *person1 = this->getNodeProfileWithEmail(email1);
    Node<Profile> *person2 = this->getNodeProfileWithEmail(email2);
    
    person1->data.addFriend(&person2->data);
    person2->data.addFriend(&person1->data);
    //std::cout << "===returning from makeFriends(" << email1 << ", " << email2 << ")\n";
}

void SocialNetwork::likePost(int postId, const std::string &email) {
    
    Node<Post> *tmp;
    Node<Profile> *liker = this->getNodeProfileWithEmail(email);
    
    for (tmp = this->posts.getFirstNode(); tmp; tmp = tmp->next) {
        if (tmp->data.getPostId() == postId) {
            break;
        }
    }
    
    liker->data.addLike(&tmp->data);
    //std::cout << "===returning from likePost(" << postId << ", " << email << ")\n";
}

void SocialNetwork::unlikePost(int postId, const std::string &email) {
    
    Node<Post> *tmp;
    Node<Profile> *unliker = this->getNodeProfileWithEmail(email);
    
    for (tmp = this->posts.getFirstNode(); tmp; tmp = tmp->next) {
        if (tmp->data.getPostId() == postId) {
            break;
        }
    }
    
    unliker->data.dropLike(&tmp->data);
    //std::cout << "===returning from unlikePost(" << postId << ", " << email << ")\n";
    
    /*
    Node<Post *> *tmpp;
    
    for (tmpp = unliker->data.getLikes().getFirstNode(); tmpp; tmpp = tmpp->next) {
        if (tmpp->data->getPostId() == postId) {
            break;
        }
    }
    
    unliker->data.getLikes().removeNode(tmpp);
    */
}

LinkedList<Profile *> SocialNetwork::getMutualFriends(const std::string &email1, const std::string &email2) {
    
    LinkedList<Profile *> mutualFriends;
    Node<Profile *> *friendOfPerson1 = this->getNodeProfileWithEmail(email1)->data.getFriends().getFirstNode();
    LinkedList<Profile *> friendsOfPerson2 = this->getNodeProfileWithEmail(email2)->data.getFriends();
    
    for (; friendOfPerson1; friendOfPerson1 = friendOfPerson1->next) {
        if (friendsOfPerson2.getNode(friendOfPerson1->data)) {
            mutualFriends.insertAtTheTail(friendOfPerson1->data);
        }
    }
    //std::cout << "===returning from getMutualFriends(" << email1 << ", " << email2 << ")\n";
    return mutualFriends;
}

LinkedList<Post *> SocialNetwork::getListOfMostRecentPosts(const std::string &email, int k) {
    
    LinkedList<Post *> recentPosts;
    //std::cout << "===returning from getListOfMostRecentPosts(" << email << ", " << k << ")\n";
    if (k) {
        Node<Post *> *postOfPerson = this->getNodeProfileWithEmail(email)->data.getPosts().getLastNode();
        
        for (; k && postOfPerson; postOfPerson = postOfPerson->prev, k--) {
            recentPosts.insertAtTheTail(postOfPerson->data);
        }
    }
    
    return recentPosts;
}

void SocialNetwork::print() const {
    std::cout << "# Printing the social network ..." << std::endl;

    std::cout << "# Number of profiles is " << profiles.getNumberOfNodes() << ":" << std::endl;
    profiles.print();

    std::cout << "# Number of posts is " << posts.getNumberOfNodes() << ":" << std::endl;
    posts.print();

    std::cout << "# Printing is done." << std::endl;
}

/* the helper function to find the pointer to Node<Profile> by given email */
Node<Profile> *SocialNetwork::getNodeProfileWithEmail(const std::string &email) {
    
    Node<Profile> *tmp;
    
    for (tmp = this->profiles.getFirstNode(); tmp; tmp = tmp->next) {
        if (tmp->data.getEmail() == email) {
            break;
        }
    }
    
    return tmp;
}