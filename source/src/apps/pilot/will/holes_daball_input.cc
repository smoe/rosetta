// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @brief

#include <core/types.hh>
#include <devel/init.hh>

#include <core/pose/Pose.hh>
#include <core/scoring/packing/PoseBalls.hh>



#include <basic/options/option.hh>
#include <basic/options/keys/holes.OptionKeys.gen.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>


#include <utility/io/izstream.hh>




#include <core/import_pose/import_pose.hh>
#include <utility/vector1.hh>


using std::string;
using namespace core;
using utility::vector1;
using core::id::AtomID;
using numeric::xyzVector;
using namespace core::scoring::packing;

// HolesParamsRes params("/Users/sheffler/project/holes/holes_params.txt");


void test( std::string fname ) {

	// Real m,a,d = 0.00001;
	// while( d < 1.0 ) {
	//  test_deriv(d,m,a);
	//  std::cout << d << " " << m << " " << a << std::endl;
	//  d *= 10;
	// }
	// return;

	using namespace std;
	using namespace core;
	using namespace io;
	using namespace pose;
	using namespace scoring;
	using namespace packing;
	using namespace basic::options;

	Pose pose;
	core::import_pose::pose_from_file(pose,fname, core::import_pose::PDB_file);

	int hmode = basic::options::option[ OptionKeys::holes::h_mode ](); // default 0
	PoseBalls pb( pose, hmode );

	// std::string cmd = basic::options::option[ OptionKeys::holes::dalphaball ]();
	// redi::pstream proc( cmd + " cavballs" );
	cout << "NPOINTS" << endl << pb.nballs() << endl << "COORDS" << endl;
	for ( Size i = 1; i <= pb.nballs(); i++ ) {
		Ball & b(pb.ball(i));
		//  // cout << "PoseBalls " << i << " " << pb.index_to_id(i) << " " << b.x() << " " << b.y() << " " << b.z() << " " << b.r() <<  endl;
		cout << b.x() << " " << b.y() << " " << b.z() << " " << b.r() << " " << endl;
	}
	cout << "END" << endl;
	fclose( stdout );


}

int
main (int argc, char *argv[])
{

	try {


		devel::init( argc, argv );

		using namespace basic::options;
		using namespace basic::options::OptionKeys;
		using namespace utility;

		// test_gradient();
		// return 0;

		if ( option[ in::file::s ].user() ) {
			vector1<file::FileName> files( option[ in::file::s ]() );
			for ( size_t i = 1; i <= files.size(); ++i ) {
				test( files[i] );
			}
		} else if ( option[ in::file::l ].user() ) {
			vector1<file::FileName> files( option[ in::file::l ]() );
			for ( size_t i = 1; i <= files.size(); ++i ) {
				utility::io::izstream list( files[i] );
				std::string fname;
				while ( list >> fname ) {
					// std::cerr << "'" << fname << "'" << std::endl;
					test( fname );
				}
			}
		}

		return 0;


	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}

}
