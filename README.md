# sortedvector
STL std::vector with known/maintainedand customisable sorted state

Example usage:
  A function is passed a const std::vector<std::shared_ptr<person>> without additional data describing its sorted state and the function requires the elements to be sorted using the 'height' field of the 'person' instances. To be certain of its sorted state the function would have to create a mutable version of the std::vector and then sort as required. If though the std::vector could have be queried to see if it is already sorted as required no cloning/sorting would have been required.

MFC app showing  usage of vector/sort code, more details can be found here:

https://youtu.be/EYUQKjgAgPM
