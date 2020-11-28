#include "ast.hpp"
#include "type.hpp"
int TArray::getLen() {
    if (this->len_->getValue() == 0) {
        return 0;
    }
    return this->getWidth() / (this->len_->getValue() * 4);
}

int TArray::getWidth() {
    return this->base_type_->getWidth() * len_->getValue();
}