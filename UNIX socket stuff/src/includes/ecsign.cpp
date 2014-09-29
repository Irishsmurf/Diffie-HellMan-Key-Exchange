/*
 *   Elliptic Curve Digital Signature Algorithm (ECDSA)
 *
 *
 *   This program asks for the name of a <file>, computes its message digest,
 *   signs it, and outputs the signature to a file <file>.ecs. It is assumed 
 *   that curve parameters are available from a file common.ecs, as well as 
 *   the private key of the signer previously generated by the ecsgen program
 *
 *   The curve is y^2=x^3+Ax+B mod p
 *
 *   The file common.ecs is presumed to exist, and to contain the domain
 *   information {p,A,B,q,x,y}, where A and B are curve parameters, (x,y) are
 *   a point of order q, p is the prime modulus, and q is the order of the 
 *   point (x,y). In fact normally q is the prime number of points counted
 *   on the curve. 
 *
 *   Requires: big.cpp ecn.cpp
 *
 *   Copyright (c) 1997-2003 Shamus Software Ltd.
 */

#include <iostream>
#include <cstring>
#include <fstream>
#include "ecn.h"

using namespace std;

#ifndef MR_NOFULLWIDTH
Miracl precision(200,256);
#else
Miracl precision(50,MAXBASE);
#endif

void strip(char *name)
{ /* strip off filename extension */
    int i;
    for (i=0;name[i]!='\0';i++)
    {
        if (name[i]!='.') continue;
        name[i]='\0';
        break;
    }
}

static Big hash(ifstream &fp)
{ /* compute hash function */
    char ch,s[20];
    Big h;
    sha sh;
    shs_init(&sh);
    forever 
    { /* read in bytes from message file */
        fp.get(ch);
        if (fp.eof()) break;
        shs_process(&sh,ch);
    }
    shs_hash(&sh,s);
    h=from_binary(20,s);
    return h;
}

int main()
{
    ifstream common("common.ecs");    /* construct file I/O streams */
    ifstream private_key("private.ecs");
    ifstream message;
    ofstream signature;
    char ifname[50],ofname[50];
    ECn G;
    Big a,b,p,q,x,y,h,r,s,d,k;
    long seed;
    int bits;
    miracl *mip=&precision;

/* randomise */
    cout << "Enter 9 digit random number seed  = ";
    cin >> seed;
    irand(seed);

/* get common data */
    common >> bits;
    mip->IOBASE=16;
    common >> p >> a >> b >> q >> x >> y;
    mip->IOBASE=10;

/* calculate r - this can be done off-line,
   and hence amortized to almost nothing    */
    ecurve(a,b,p,MR_PROJECTIVE);
    G=ECn(x,y);
    k=rand(q);
    G*=k;            /* see ebrick.cpp for technique to speed this up */
    G.get(r);
    r%=q;

/* get private key of recipient */
    private_key >> d;

/* get message */
    cout << "file to be signed = " ;
    cin >> ifname;
    strcpy(ofname,ifname);
    strip(ofname);
    strcat(ofname,".ecs");
    message.open(ifname,ios::binary|ios::in); 
    if (!message)
    {
        cout << "Unable to open file " << ifname << "\n";
        return 0;
    }
    h=hash(message);

/* calculate s */
    k=inverse(k,q);
    s=((h+d*r)*k)%q;
    signature.open(ofname);
    signature << r << endl;
    signature << s << endl;
    return 0;
}

