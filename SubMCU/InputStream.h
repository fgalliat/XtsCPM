/**
 * 
 * Input Stream interface (see also Arduino::Stream class)
 * 
 * Xtase - fgalliat @Dec2018
 * 
 * 
 */

#ifndef __INPUTSTREAM_H__
#define __INPUTSTREAM_H__ 1

class InputStream {
  public:
    virtual void init(bool reinitBuffer=true);
    virtual void poll();

    virtual int available();
    virtual int read();
    virtual char* readLine();
};

#endif