/*
 *   Elliptic Curve Digital Signature Algorithm (ECDSA)
 *
 *
 *   This program verifies the signature given to a <file> in
 *   <file>.ecs generated by program ecsign
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
#include <fstream>
#include <cstring>
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
    ifstream public_key("public.ecs");
    ifstream message;
    ifstream signature;
    ECn G,Pub;
    int bits,ep;
    Big a,b,p,q,x,y,v,u1,u2,r,s,h;
    char ifname[50],ofname[50];
    miracl *mip=&precision;

/* get public data */
    common >> bits;
    mip->IOBASE=16;
    common >> p >> a >> b >> q >> x >> y;
    mip->IOBASE=10;
    ecurve(a,b,p,MR_PROJECTIVE);
    G=ECn(x,y);
/* get public key of signer */
    public_key >> ep >> x;
    Pub=ECn(x,ep);         // decompress
/* get message */
    cout << "signed file = " ;
    cin.sync();
    cin.getline(ifname,13);
    strcpy(ofname,ifname);
    strip(ofname);
    strcat(ofname,".ecs");
    message.open(ifname,ios::binary|ios::in); 
    if (!message)
    { /* no message */
        cout << "Unable to open file " << ifname << "\n";
        return 0;
    }
    h=hash(message);

    signature.open(ofname,ios::in);
    if (!signature)
    { /* no signature */
        cout << "signature file " << ofname << " does not exist\n";
        return 0;
    }
    signature >> r >> s;
    if (r>=q || s>=q)
    {
        cout << "Signature is NOT verified\n";
        return 0;
    }
    s=inverse(s,q);
    u1=(h*s)%q;
    u2=(r*s)%q;

    G=mul(u2,Pub,u1,G);
    G.get(v);
    v%=q;
    if (v==r) cout << "Signature is verified\n";
    else      cout << "Signature is NOT verified\n";
    return 0;
}

