// Date:    21st August 2019
// Purpose: Testing utility for behave library.
//
// Version control
// 21 Aug 2019 Duncan Camilleri           Initial development
//

#include <sys/time.h>
#include <stdio.h>
#include <list>
#include <map>
#include <string>
#include <random>

#include <helpers.h>
#include "objref.h"
#include "stringlist.h"
#include "weightedbinary.h"
#include "serializer.h"
#include "mood.h"
#include "action.h"
#include "being.h"
#include "gathering.h"
#include "behavefactory.h"

using namespace std;

//
// Experiment in human behaviour.
// In this experiment we have the following people:
// Bob is a tradesman. He can lay bricks, concrete yards, landscape, plaster and
//    paint.
// Jane is a sales executive. She sells books, accounting services and also
//    office equipment.
// Peter is a computer programmer. He enjoys playing computer games and also 
//    hacking away at a computer system.
// Alice is a free soul. She only cares about the moment and works in retail.
//    She loves playing guitar and piano and also enjoys rock bands
//
// A bit of background will be provided for each person along with some
// some information on the underlying model using Plutchik's Wheel of emotions
// implemented in this experiment. Bob's background will provide a basis of the
// behaviour of this model.

// To come up with moods, actions, beings and gatherings for the model, some
// thinking  in the way we humans live life and the psychology behind it is
// required.
//
// Bob:
// For Bob to lay a brick, he needs to a trigger emotion which will cause him
// to take that action and also leave a reactional impact on his emotions.
// For this example, we assume that Bob has become a tradesman because he 
// finds that laying bricks brings some form of emotional response that counters
// the experiences that bring/brought sadness into his life. See, the thing
// with Bob is that when he was young he fell deeply in love with a young maiden
// who cheated on him and left him for some other dude. Being a creative person,
// Bob found his passion of brick work when he was with his father working on
// their house. Bob found that the process of laying bricks helped him feel
// relieved of the loss and he kind of kept going with it. This has brought
// him great joy. Therefore we can say that for Bob, laying bricks can be
// represented by the following triggers and reactions:
// triggers: sadness, reactions: joy
// For short these will be represented as action(sadness => joy)
// A bit more about Bob is that he has a close relationship with his father
// whose had a loss (his wife). This means that his father was protective
// and loving to Bob, maintained a good balance of love, camaraderie, trust and
// freedom. This has made Bob (who lost his mother) be of a great example to
// others' needs as he is sensitive and understanding of hardship. As a result,
// he also is a person who gets things done as he has grown to love brickwork.
// We can generally say that Bob's 'starting' mood (based only on the above
// conditions) are as follows:
// mood(joy:-0.25,trust:0.7,fear:0.4,surprise:0.12,sadness:0.4,disgust:0.2,
//       anger:0.38,anticipation:0.2)
// Bob's level of trust in himself and his father has led him forward to
// expand on his skillset and he also became proficient in concreting yards,
// landscaping, plastering as well as painting. This has brought in more
// confidence and passion within him because he was more joyful, even more
// trusting, less fearful, and a bit more surprised as what he achieved
// felt really good.
// This has shifted his mood as follows:
// mood(joy:+0.4,trust:+0.2,fear:-0.25,surprise:+0.2,sadness:-0.5,disgust:-0.6,
//    anger:-0.5,anticipation:+0.02)
// Following this, Bob's mood becomes:
// mood(joy:0.15,trust:0.9,fear:0.15,surprise:0.32,sadness:-0.1.disgust:-0.4,
//    anger:-0.12,anticipation:0.22)
// As a result, we can say that Bob's general mood has shifted from one mood
// to the other. An action was triggered because of his emotions which
// has left him with a reactive emotional response. This can subsequently
// trigger more actions to be taken, forming a chain reaction of events and
// emotional shifts.
// The model implemented below is based on the above example. The following
// are the remaining contestants.
//
// Jane:
// Jane has been brought up under a strict father who wanted her to succeed
// in life and therefore has been pushed to achieve a lot of targets. Her
// mother also instilled in her the importance of success and as a result
// Jane hasn't really focused much on relationships. Jane has been focusing
// on selling products and built her knowledge of accounting while she was
// working for a small business. Jane's initial disposition is:
// mood(joy:-0.18,trust:0.28,fear:0.35,surprise:-0.2,sadness:0.004.disgust:-0.7,
//    anger:-0.4,anticipation:0.32)
//
// Peter:
// Peter has been bullied when he was much younger and as a result, he ended
// up not socializing a lot. He resorted to the life of computers and when
// he was young found pleasure in trying to break into computer systems
// or build new models. As a result he has a lot of experience in his field and
// knows his stuff. He is considered an elite in his field. Peter remained
// single because he grew up as a reserved person. Peter's initial disposition
// is:
// mood(joy:-0.4,trust:0.2,fear:0.56,surprise:-0.4,sadness:-0.4.disgust:0.002,
//    anger:0.64,anticipation:0.6)
//
// Alice:
// Alice had a lot of freedom when she was young and was allowed to pursue all
// that she desired. She played guitar, rode bikes, travelled, and done a lot
// of fun stuff. She is a little bit of an entrepreneur and has been trying to
// set up a business of her own by selling some home made candles, and other
// paraphernalia. While her financial health is not very strong, she still
// enjoys a wealthy life emotionally and that is what's most important for her.
// Alice's initial disposition is as follows:
// mood(joy:0.6,trust:0.002,fear:0.2,surprise:0.4,sadness:0.158,disgust:-0.72,
//    anger:-0.42,anticipation:0.463)


BehaveFactory gBehave;
const char* const gXml =
   "/home/duncan/dev-cpp/src/experimental/ai/behave/config.xml";

// 
//           |
// ,---.,---.|--- .   .,---.
// `---.|---'|    |   ||   |
// `---'`---'`---'`---'|---'
//                     |
// 

bool setWorldUp()
{
   if (!gBehave.load(gXml))
      return false;

   // Done.
   return true;
   
}



// 
//           o
// ,-.-.,---..,---.    ,---.,---.,---.
// | | |,---|||   |    ,---||   ||   |
// ` ' '`---^``   '    `---^|---'|---'
//                          |    |
// 
#include <string.h>

void usingsl()
{
   const char* pp = "djasdhjhsadhih";
   vector<sloffset> v;
   StringList<slsiz::medium> a;
   for (int c = 0; c < 10000; ++c) {
      v.push_back(a.addString(pp, strlen(pp)));
   }
}  

void usingStr()
{
   list<string> v;
   for (int c = 0; c < 10000; ++c) {
      v.push_back("djasdhjhsadhih");
   }
}

void usingStrVect()
{
   vector<string> v;
   for (int c = 0; c < 10000; ++c) {
      v.push_back("djasdhjhsadhih");
   }
}


int main(int argc, char** argv)
{
   setWorldUp();
   gBehave.save("resaved.xml");

   return 0;
}
