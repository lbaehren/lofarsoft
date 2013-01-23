$doxydocs=
{
  classes => [
    {
      name => 'Reader_Dynspec_Part',
      includes => {
        local => 'no',
        name => 'Reader_Dynspec_Part.h'
      },
      all_members => [
        {
          name => 'readDynspec',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Reader_Dynspec_Part'
        },
        {
          name => 'Reader_Dynspec_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Reader_Dynspec_Part'
        },
        {
          name => '~Reader_Dynspec_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Reader_Dynspec_Part'
        }
      ],
      public_methods => {
        members => [
          {
            kind => 'function',
            name => 'Reader_Dynspec_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => '~Reader_Dynspec_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => 'readDynspec',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'void',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'pathFile',
                type => 'std::string'
              },
              {
                declaration_name => 'dynspecMetadata',
                type => 'Stock_Write_Dynspec_Metadata_Part *'
              },
              {
                declaration_name => 'dynspecData',
                type => 'Stock_Write_Dynspec_Data_Part *'
              },
              {
                declaration_name => 'i',
                type => 'int'
              },
              {
                declaration_name => 'j',
                type => 'int'
              },
              {
                declaration_name => 'q',
                type => 'int'
              },
              {
                declaration_name => 'obsNofSAP',
                type => 'int'
              },
              {
                declaration_name => 'obsNofSource',
                type => 'int'
              },
              {
                declaration_name => 'obsNofFrequencyBand',
                type => 'int'
              },
              {
                declaration_name => 'obsNofStockes',
                type => 'int'
              },
              {
                declaration_name => 'stokesComponent',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'SAPindex',
                type => 'int'
              },
              {
                declaration_name => 'timeMinSelect',
                type => 'float'
              },
              {
                declaration_name => 'timeMaxSelect',
                type => 'float'
              },
              {
                declaration_name => 'timeRebin',
                type => 'float'
              },
              {
                declaration_name => 'frequencyMin',
                type => 'float'
              },
              {
                declaration_name => 'frequencyMax',
                type => 'float'
              },
              {
                declaration_name => 'frequencyRebin',
                type => 'float'
              }
            ]
          }
        ]
      },
      brief => {},
      detailed => {}
    },
    {
      name => 'Reader_Root_Part',
      includes => {
        local => 'no',
        name => 'Reader_Root_Part.h'
      },
      all_members => [
        {
          name => 'Reader_Root_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Reader_Root_Part'
        },
        {
          name => 'readRoot',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Reader_Root_Part'
        },
        {
          name => '~Reader_Root_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Reader_Root_Part'
        }
      ],
      public_methods => {
        members => [
          {
            kind => 'function',
            name => 'Reader_Root_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => '~Reader_Root_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => 'readRoot',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'void',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'pathFile',
                type => 'std::string'
              },
              {
                declaration_name => 'obsName',
                type => 'std::string'
              },
              {
                declaration_name => 'rootMetadata',
                type => 'Stock_Write_Root_Metadata_Part *'
              },
              {
                declaration_name => 'obsNofSource',
                type => 'int'
              },
              {
                declaration_name => 'obsNofBeam',
                type => 'int'
              },
              {
                declaration_name => 'obsNofFrequencyBand',
                type => 'int'
              },
              {
                declaration_name => 'obsNofTiledDynspec',
                type => 'int'
              },
              {
                declaration_name => 'SAPname',
                type => 'std::string'
              },
              {
                declaration_name => 'SAPindex',
                type => 'int'
              },
              {
                declaration_name => 'timeMinSelect',
                type => 'float'
              },
              {
                declaration_name => 'timeMaxSelect',
                type => 'float'
              },
              {
                declaration_name => 'timeRebin',
                type => 'float'
              },
              {
                declaration_name => 'frequencyMin',
                type => 'float'
              },
              {
                declaration_name => 'frequencyMax',
                type => 'float'
              },
              {
                declaration_name => 'frequencyRebin',
                type => 'float'
              }
            ]
          }
        ]
      },
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Dynspec_Data_Part',
      includes => {
        local => 'no',
        name => 'Stock_Write_Dynspec_Data_Part.h'
      },
      all_members => [
        {
          name => 'm_Nspectral',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Data_Part'
        },
        {
          name => 'm_NspectralReal',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Data_Part'
        },
        {
          name => 'm_Ntime',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Data_Part'
        },
        {
          name => 'm_NtimeReal',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Data_Part'
        },
        {
          name => 'Stock_Write_Dynspec_Data_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Dynspec_Data_Part'
        },
        {
          name => 'stockDynspecData',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Dynspec_Data_Part'
        },
        {
          name => 'writeDynspecData',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Dynspec_Data_Part'
        },
        {
          name => '~Stock_Write_Dynspec_Data_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Dynspec_Data_Part'
        }
      ],
      public_methods => {
        members => [
          {
            kind => 'function',
            name => 'Stock_Write_Dynspec_Data_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => '~Stock_Write_Dynspec_Data_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => 'stockDynspecData',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'void',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'Ntime',
                type => 'int'
              },
              {
                declaration_name => 'Nspectral',
                type => 'int'
              },
              {
                declaration_name => 'NtimeReal',
                type => 'int'
              },
              {
                declaration_name => 'NspectralReal',
                type => 'int'
              }
            ]
          },
          {
            kind => 'function',
            name => 'writeDynspecData',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'void',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'dynspec_grp',
                type => 'Group &'
              },
              {
                declaration_name => 'pathDir',
                type => 'std::string'
              },
              {
                declaration_name => 'obsName',
                type => 'std::string'
              },
              {
                declaration_name => 'pathFile',
                type => 'std::string'
              },
              {
                declaration_name => 'outputFile',
                type => 'std::string'
              },
              {
                declaration_name => 'root_grp',
                type => 'File &'
              },
              {
                declaration_name => 'i',
                type => 'int'
              },
              {
                declaration_name => 'j',
                type => 'int'
              },
              {
                declaration_name => 'k',
                type => 'int'
              },
              {
                declaration_name => 'l',
                type => 'int'
              },
              {
                declaration_name => 'q',
                type => 'int'
              },
              {
                declaration_name => 'obsNofStockes',
                type => 'int'
              },
              {
                declaration_name => 'stokesComponent',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'memoryRAM',
                type => 'float'
              },
              {
                declaration_name => 'SAPindex',
                type => 'int'
              },
              {
                declaration_name => 'timeMinSelect',
                type => 'float'
              },
              {
                declaration_name => 'timeMaxSelect',
                type => 'float'
              },
              {
                declaration_name => 'timeRebin',
                type => 'float'
              },
              {
                declaration_name => 'frequencyMin',
                type => 'float'
              },
              {
                declaration_name => 'frequencyMax',
                type => 'float'
              },
              {
                declaration_name => 'frequencyRebin',
                type => 'float'
              }
            ]
          }
        ]
      },
      private_members => {
        members => [
          {
            kind => 'variable',
            name => 'm_Ntime',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_Nspectral',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_NtimeReal',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_NspectralReal',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          }
        ]
      },
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Dynspec_Metadata_Part',
      includes => {
        local => 'no',
        name => 'Stock_Write_Dynspec_Metadata_Part.h'
      },
      all_members => [
        {
          name => 'm_attr_100',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_101',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_102',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_103',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_104',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_105',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_106',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_107',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_108',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_109',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_110',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_111',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_112',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_113',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_114',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_115',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_116',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_117',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_118',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_119',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_123',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_124',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_125',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_126',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_127',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_128',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_129',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_130',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_131',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_132',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_133',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_135',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_137',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_138',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_139',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_140',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_141',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_142',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_147',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_148',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_149',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_150',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_151',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_152',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_153',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_154',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_155',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_156',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_157',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_158',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_159',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_160',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_161',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_162',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_163',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_164',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_165',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_166',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_167',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_168',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_69',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_70',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_71',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_72',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_73',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_76',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_77',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_78',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_79',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_80',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_81',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_82',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_83',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_84',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_85',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_86',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_87',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_88',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_89',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_90',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_91',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_92',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_93',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_94',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_95',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_96',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_97',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_98',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_attr_99',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_Nspectral',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_Ntime',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_101',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_102',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_109',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_112',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_114',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_115',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_116',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_117',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_118',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_119',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_125',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_127',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_128',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_129',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_130',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_131',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_132',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_133',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_135',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_139',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_141',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_142',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_153',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_154',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_155',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_156',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_157',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_158',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_92',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'm_size_attr_97',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'Stock_Write_Dynspec_Metadata_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'stockDynspecMetadata',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => 'writeDynspecMetadata',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        },
        {
          name => '~Stock_Write_Dynspec_Metadata_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Dynspec_Metadata_Part'
        }
      ],
      public_methods => {
        members => [
          {
            kind => 'function',
            name => 'Stock_Write_Dynspec_Metadata_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => '~Stock_Write_Dynspec_Metadata_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => 'stockDynspecMetadata',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'void',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'attr_69',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_70',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_71',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_72',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_73',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_76',
                type => 'double'
              },
              {
                declaration_name => 'attr_77',
                type => 'double'
              },
              {
                declaration_name => 'attr_78',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_79',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_80',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_81',
                type => 'double'
              },
              {
                declaration_name => 'attr_82',
                type => 'double'
              },
              {
                declaration_name => 'attr_83',
                type => 'double'
              },
              {
                declaration_name => 'attr_84',
                type => 'double'
              },
              {
                declaration_name => 'attr_85',
                type => 'double'
              },
              {
                declaration_name => 'attr_86',
                type => 'double'
              },
              {
                declaration_name => 'attr_87',
                type => 'double'
              },
              {
                declaration_name => 'attr_88',
                type => 'double'
              },
              {
                declaration_name => 'attr_89',
                type => 'double'
              },
              {
                declaration_name => 'attr_90',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_91',
                type => 'double'
              },
              {
                declaration_name => 'attr_92',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_93',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_94',
                type => 'double'
              },
              {
                declaration_name => 'attr_95',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_96',
                type => 'bool'
              },
              {
                declaration_name => 'attr_97',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_98',
                type => 'bool'
              },
              {
                declaration_name => 'attr_99',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_100',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_101',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_102',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_103',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_104',
                type => 'double'
              },
              {
                declaration_name => 'attr_105',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_106',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_107',
                type => 'double'
              },
              {
                declaration_name => 'attr_108',
                type => 'double'
              },
              {
                declaration_name => 'attr_109',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_110',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_111',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_112',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_113',
                type => 'int'
              },
              {
                declaration_name => 'attr_114',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_115',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_116',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_117',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_118',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_119',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_123',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_124',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_125',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_126',
                type => 'int'
              },
              {
                declaration_name => 'attr_127',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_128',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_129',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_130',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_131',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_132',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_133',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_135',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_137',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_138',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_139',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_140',
                type => 'int'
              },
              {
                declaration_name => 'attr_141',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_142',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'Ntime',
                type => 'int'
              },
              {
                declaration_name => 'Nspectral',
                type => 'int'
              },
              {
                declaration_name => 'attr_147',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_148',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_149',
                type => 'int'
              },
              {
                declaration_name => 'attr_150',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_151',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_152',
                type => 'int'
              },
              {
                declaration_name => 'attr_153',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_154',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_155',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_156',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_157',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_158',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_159',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_160',
                type => 'bool'
              },
              {
                declaration_name => 'attr_161',
                type => 'bool'
              },
              {
                declaration_name => 'attr_162',
                type => 'bool'
              },
              {
                declaration_name => 'attr_163',
                type => 'bool'
              },
              {
                declaration_name => 'attr_164',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_165',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_166',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_167',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_168',
                type => 'std::string'
              }
            ]
          },
          {
            kind => 'function',
            name => 'writeDynspecMetadata',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'void',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'dynspec_grp',
                type => 'Group &'
              },
              {
                declaration_name => 'pathDir',
                type => 'std::string'
              },
              {
                declaration_name => 'obsName',
                type => 'std::string'
              },
              {
                declaration_name => 'pathFile',
                type => 'std::string'
              },
              {
                declaration_name => 'outputFile',
                type => 'std::string'
              },
              {
                declaration_name => 'root_grp',
                type => 'File &'
              },
              {
                declaration_name => 'i',
                type => 'int'
              },
              {
                declaration_name => 'j',
                type => 'int'
              },
              {
                declaration_name => 'k',
                type => 'int'
              },
              {
                declaration_name => 'l',
                type => 'int'
              },
              {
                declaration_name => 'q',
                type => 'int'
              },
              {
                declaration_name => 'obsNofStockes',
                type => 'int'
              },
              {
                declaration_name => 'stokesComponent',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'memoryRAM',
                type => 'float'
              }
            ]
          }
        ]
      },
      private_members => {
        members => [
          {
            kind => 'variable',
            name => 'm_attr_69',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_70',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_71',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_72',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_73',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_76',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_77',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_78',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_79',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_80',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_81',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_82',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_83',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_84',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_85',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_86',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_87',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_88',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_89',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_90',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_91',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_92',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_92',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_93',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_94',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_95',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_96',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_97',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_97',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_98',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool'
          },
          {
            kind => 'variable',
            name => 'm_attr_99',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_100',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_101',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_101',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_102',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_102',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_103',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_104',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_105',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_106',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_107',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_108',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_109',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_109',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_110',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_111',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_112',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_112',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_113',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_114',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_114',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_115',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_115',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_116',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_116',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_117',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_117',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_118',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_118',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_119',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_119',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_attr_123',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_124',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_125',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_125',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_126',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_127',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_127',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_128',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_128',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_129',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_129',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_130',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_130',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_131',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_131',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_132',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_132',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_133',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_133',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_135',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_135',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_attr_137',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_138',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_139',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_139',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_140',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_141',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_141',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_142',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_142',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_Ntime',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_Nspectral',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_147',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_148',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_149',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_150',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_151',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_152',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_153',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_153',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_154',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_154',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_155',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_155',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_156',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_156',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_157',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_157',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_158',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_158',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_159',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_160',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool'
          },
          {
            kind => 'variable',
            name => 'm_attr_161',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool'
          },
          {
            kind => 'variable',
            name => 'm_attr_162',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool'
          },
          {
            kind => 'variable',
            name => 'm_attr_163',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool'
          },
          {
            kind => 'variable',
            name => 'm_attr_164',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_165',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_166',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_167',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_168',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          }
        ]
      },
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Root_Metadata_Part',
      includes => {
        local => 'no',
        name => 'Stock_Write_Root_Metadata_Part.h'
      },
      all_members => [
        {
          name => 'm_attr_1',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_10',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_11',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_12',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_13',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_14',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_16',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_17',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_19',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_2',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_20',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_21',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_22',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_23',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_24',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_25',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_26',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_27',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_28',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_29',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_3',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_30',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_31',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_32',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_33',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_34',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_35',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_36',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_37',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_38',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_39',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_4',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_40',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_41',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_42',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_45',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_46',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_47',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_48',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_49',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_5',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_52',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_53',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_54',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_55',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_56',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_57',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_58',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_59',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_60',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_61',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_62',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_63',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_64',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_65',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_66',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_67',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_68',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_7',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_8',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_attr_9',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_size_attr_20',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_size_attr_48',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_size_attr_49',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_size_attr_65',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_size_attr_66',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_size_attr_67',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'm_size_attr_68',
          virtualness => 'non_virtual',
          protection => 'private',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'Stock_Write_Root_Metadata_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'stockRootMetadata',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => 'writeRootMetadata',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Root_Metadata_Part'
        },
        {
          name => '~Stock_Write_Root_Metadata_Part',
          virtualness => 'non_virtual',
          protection => 'public',
          scope => 'Stock_Write_Root_Metadata_Part'
        }
      ],
      public_methods => {
        members => [
          {
            kind => 'function',
            name => 'Stock_Write_Root_Metadata_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => '~Stock_Write_Root_Metadata_Part',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            const => 'no',
            volatile => 'no',
            parameters => [
            ]
          },
          {
            kind => 'function',
            name => 'stockRootMetadata',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'void',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'attr_1',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_2',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_3',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_4',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_5',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_7',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_8',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_9',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_10',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_11',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_12',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_13',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_14',
                type => 'double'
              },
              {
                declaration_name => 'attr_16',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_17',
                type => 'double'
              },
              {
                declaration_name => 'attr_19',
                type => 'int'
              },
              {
                declaration_name => 'attr_20',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_21',
                type => 'double'
              },
              {
                declaration_name => 'attr_22',
                type => 'double'
              },
              {
                declaration_name => 'attr_23',
                type => 'double'
              },
              {
                declaration_name => 'attr_24',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_25',
                type => 'int'
              },
              {
                declaration_name => 'attr_26',
                type => 'double'
              },
              {
                declaration_name => 'attr_27',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_28',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_29',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_30',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_31',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_32',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_33',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_34',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_35',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_36',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_37',
                type => 'bool'
              },
              {
                declaration_name => 'attr_38',
                type => 'int'
              },
              {
                declaration_name => 'attr_39',
                type => 'int'
              },
              {
                declaration_name => 'attr_40',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_41',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_42',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_45',
                type => 'double'
              },
              {
                declaration_name => 'attr_46',
                type => 'double'
              },
              {
                declaration_name => 'attr_47',
                type => 'double'
              },
              {
                declaration_name => 'attr_48',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_49',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_52',
                type => 'int'
              },
              {
                declaration_name => 'attr_53',
                type => 'double'
              },
              {
                declaration_name => 'attr_54',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_55',
                type => 'double'
              },
              {
                declaration_name => 'attr_56',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_57',
                type => 'double'
              },
              {
                declaration_name => 'attr_58',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_59',
                type => 'int'
              },
              {
                declaration_name => 'attr_60',
                type => 'double'
              },
              {
                declaration_name => 'attr_61',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_62',
                type => 'double'
              },
              {
                declaration_name => 'attr_63',
                type => 'std::string'
              },
              {
                declaration_name => 'attr_64',
                type => 'double'
              },
              {
                declaration_name => 'attr_65',
                type => 'std::vector< std::string >'
              },
              {
                declaration_name => 'attr_66',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_67',
                type => 'std::vector< double >'
              },
              {
                declaration_name => 'attr_68',
                type => 'std::vector< double >'
              }
            ]
          },
          {
            kind => 'function',
            name => 'writeRootMetadata',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'void',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'outputFile',
                type => 'std::string'
              },
              {
                declaration_name => 'root_grp',
                type => 'File &'
              }
            ]
          }
        ]
      },
      private_members => {
        members => [
          {
            kind => 'variable',
            name => 'm_attr_1',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_2',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_3',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_4',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_5',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_7',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_8',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_9',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_10',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_11',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_12',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_13',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_14',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_16',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_17',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_19',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_20',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_20',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_21',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_22',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_23',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_24',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_25',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_26',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_27',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_28',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_29',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_30',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_31',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_32',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_33',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_34',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_35',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_36',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_37',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool'
          },
          {
            kind => 'variable',
            name => 'm_attr_38',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_39',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_40',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_41',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_42',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_45',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_46',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_47',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_48',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_48',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_49',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_49',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_attr_52',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_53',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_54',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_55',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_56',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_57',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_58',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_59',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_60',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_61',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_62',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_attr_63',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::string'
          },
          {
            kind => 'variable',
            name => 'm_attr_64',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'double'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_65',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_65',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< std::string >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_66',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_66',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_67',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_67',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          },
          {
            kind => 'variable',
            name => 'm_size_attr_68',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int'
          },
          {
            kind => 'variable',
            name => 'm_attr_68',
            virtualness => 'non_virtual',
            protection => 'private',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'std::vector< double >'
          }
        ]
      },
      brief => {},
      detailed => {}
    }
  ],
  namespaces => [
    {
      name => 'dal',
      classes => [
      ],
      namespaces => [
      ],
      brief => {},
      detailed => {
        doc => [
          {
            type => 'text',
            content => 'Algorithm overview'
          },
          {
            type => 'parbreak'
          },
          {
            type => 'text',
            content => 'This code (pipeline) uses several classes. Each classes has a particular role for processing ICD3 data to dynamic spectrum data (ICD6) First, main code etablishes the principal parameter of the observation (number of SAP, Beam and Stokes parameters. Thanks to robustess nomenclature of ICD3 format, with these parameter we are able to know which file we have to process for a given SAP, Beam and Stokes parameter) After existency tests and principal parameters determination, the main code will loop of the SAPs that it has to process:'
          },
          {
            type => 'parbreak'
          },
          {
            type => 'text',
            content => 'First the code wil stok Root group metadata. Root Metadata are red by a function named readRoot and contained in the class '
          },
          {
            type => 'url',
            link => '_reader___root___part_8h',
            content => 'Reader_Root_Part.h'
          },
          {
            type => 'text',
            content => ', the Root group is generated and Root\'s matadata are stoked, then written by a function named writeRootMetadata and contained in the class '
          },
          {
            type => 'url',
            link => '_stock___write___root___metadata___part_8h',
            content => 'Stock_Write_Root_Metadata_Part.h'
          },
          {
            type => 'parbreak'
          },
          {
            type => 'text',
            content => 'Secondly, the main code will loop on Beam, and generate dynamic spectrum group and its metadata. The dynamic spectrum\'s metadata are red by a function named readDynspec and contained in the class '
          },
          {
            type => 'url',
            link => '_reader___dynspec___part_8h',
            content => 'Reader_Dynspec_Part.h'
          },
          {
            type => 'text',
            content => ', after, metadata are stoked, then written by a function named writeDynspecMetadata contained in '
          },
          {
            type => 'url',
            link => '_stock___write___dynspec___metadata___part_8h',
            content => 'Stock_Write_Dynspec_Metadata_Part.h'
          },
          {
            type => 'parbreak'
          },
          {
            type => 'text',
            content => 'Finaly, data are processed. Because LOFAR data are very voluminous, we have to develop a strategy for: To avoid swaping To have enough memory for loading data etc ... We decide to use a frozen quantity of RAM for a processing. This quantity is chosen by the user and corresponds to a number of time bins that we can process for this RAM. So, the code will loop of the number of Time series we need to process all the selected data. Rebinning is done inside these blocks. To conclude, data processing is done time blocks by time blocks, and data generation is done by the function writeDynspecData which is included in the class '
          },
          {
            type => 'url',
            link => '_stock___write___dynspec___data___part_8h',
            content => 'Stock_Write_Dynspec_Data_Part.h'
          }
        ]
      }
    },
    {
      name => 'std',
      classes => [
      ],
      namespaces => [
      ],
      brief => {},
      detailed => {}
    }
  ],
  files => [
    {
      name => 'DynspecPart.cpp',
      includes => [
        {
          name => 'iostream'
        },
        {
          name => 'string'
        },
        {
          name => 'fstream'
        },
        {
          name => 'time.h'
        },
        {
          name => 'unistd.h'
        },
        {
          name => 'Reader_Root_Part.h',
          ref => '_reader___root___part_8h'
        },
        {
          name => 'Stock_Write_Root_Metadata_Part.h',
          ref => '_stock___write___root___metadata___part_8h'
        },
        {
          name => 'Reader_Dynspec_Part.h',
          ref => '_reader___dynspec___part_8h'
        },
        {
          name => 'Stock_Write_Dynspec_Metadata_Part.h',
          ref => '_stock___write___dynspec___metadata___part_8h'
        },
        {
          name => 'Stock_Write_Dynspec_Data_Part.h',
          ref => '_stock___write___dynspec___data___part_8h'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
      ],
      functions => {
        members => [
          {
            kind => 'function',
            name => 'is_readableTer',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'file',
                type => 'const std::string &'
              }
            ]
          },
          {
            kind => 'function',
            name => 'main',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'int',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'argc',
                type => 'int'
              },
              {
                declaration_name => 'argv',
                type => 'char *',
                array => '[]'
              }
            ]
          }
        ]
      },
      brief => {
        doc => [
          {
            type => 'text',
            content => 'DAL Library 2.5 (library for hdf5 file)'
          }
        ]
      },
      detailed => {
        doc => [
          {
            type => 'text',
            content => 'Projet LOFAR'
          },
          {
            type => 'parbreak'
          },
          {
            date => [
              {
                type => 'text',
                content => '17/12/2012 '
              }
            ]
          },
          {
            author => [
              {
                type => 'text',
                content => 'Nicolas VILCHEZ '
              }
            ]
          },
          {
            version => [
              {
                type => 'text',
                content => '1.0 '
              }
            ]
          },
          {
            copyright => [
              {
                type => 'text',
                content => 'LPC2E/CNRS Station de radio-astronomie de Nançay'
              }
            ]
          },
          {
            type => 'text',
            content => 'Main program for converting ICD3 files to ICD6 (dynamic spectrum) files with (or not) a time-frequency selection and/or rebinning'
          }
        ]
      }
    },
    {
      name => 'Reader_Dynspec_Part.cpp',
      includes => [
        {
          name => 'iostream'
        },
        {
          name => 'string'
        },
        {
          name => 'fstream'
        },
        {
          name => 'Reader_Dynspec_Part.h',
          ref => '_reader___dynspec___part_8h'
        },
        {
          name => 'Stock_Write_Dynspec_Metadata_Part.h',
          ref => '_stock___write___dynspec___metadata___part_8h'
        },
        {
          name => 'Stock_Write_Dynspec_Data_Part.h',
          ref => '_stock___write___dynspec___data___part_8h'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
      ],
      functions => {
        members => [
          {
            kind => 'function',
            name => 'is_readableBis',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'file',
                type => 'const std::string &'
              }
            ]
          }
        ]
      },
      brief => {},
      detailed => {}
    },
    {
      name => 'Reader_Dynspec_Part.h',
      includes => [
        {
          name => 'string'
        },
        {
          name => 'iostream'
        },
        {
          name => 'dal/lofar/BF_File.h'
        },
        {
          name => 'Stock_Write_Dynspec_Metadata_Part.h',
          ref => '_stock___write___dynspec___metadata___part_8h'
        },
        {
          name => 'Stock_Write_Dynspec_Data_Part.h',
          ref => '_stock___write___dynspec___data___part_8h'
        }
      ],
      included_by => [
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/DynspecPart.cpp',
          ref => '_dynspec_part_8cpp'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Reader_Dynspec_Part.cpp',
          ref => '_reader___dynspec___part_8cpp'
        }
      ],
      brief => {},
      detailed => {}
    },
    {
      name => 'Reader_Root_Part.cpp',
      includes => [
        {
          name => 'iostream'
        },
        {
          name => 'string'
        },
        {
          name => 'sstream'
        },
        {
          name => 'Reader_Root_Part.h',
          ref => '_reader___root___part_8h'
        },
        {
          name => 'Stock_Write_Root_Metadata_Part.h',
          ref => '_stock___write___root___metadata___part_8h'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
      ],
      brief => {},
      detailed => {}
    },
    {
      name => 'Reader_Root_Part.h',
      includes => [
        {
          name => 'string'
        },
        {
          name => 'iostream'
        },
        {
          name => 'dal/lofar/BF_File.h'
        },
        {
          name => 'Stock_Write_Root_Metadata_Part.h',
          ref => '_stock___write___root___metadata___part_8h'
        }
      ],
      included_by => [
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/DynspecPart.cpp',
          ref => '_dynspec_part_8cpp'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Reader_Root_Part.cpp',
          ref => '_reader___root___part_8cpp'
        }
      ],
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Dynspec_Data_Part.cpp',
      includes => [
        {
          name => 'iostream'
        },
        {
          name => 'string'
        },
        {
          name => 'fstream'
        },
        {
          name => 'Stock_Write_Dynspec_Data_Part.h',
          ref => '_stock___write___dynspec___data___part_8h'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
      ],
      functions => {
        members => [
          {
            kind => 'function',
            name => 'is_readable',
            virtualness => 'non_virtual',
            protection => 'public',
            static => 'no',
            brief => {},
            detailed => {},
            type => 'bool',
            const => 'no',
            volatile => 'no',
            parameters => [
              {
                declaration_name => 'file',
                type => 'const std::string &'
              }
            ]
          }
        ]
      },
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Dynspec_Data_Part.h',
      includes => [
        {
          name => 'string'
        },
        {
          name => 'iostream'
        },
        {
          name => 'vector'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Reader_Dynspec_Part.h',
          ref => '_reader___dynspec___part_8h'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/DynspecPart.cpp',
          ref => '_dynspec_part_8cpp'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Reader_Dynspec_Part.cpp',
          ref => '_reader___dynspec___part_8cpp'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Stock_Write_Dynspec_Data_Part.cpp',
          ref => '_stock___write___dynspec___data___part_8cpp'
        }
      ],
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Dynspec_Metadata_Part.cpp',
      includes => [
        {
          name => 'iostream'
        },
        {
          name => 'string'
        },
        {
          name => 'Stock_Write_Dynspec_Metadata_Part.h',
          ref => '_stock___write___dynspec___metadata___part_8h'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
      ],
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Dynspec_Metadata_Part.h',
      includes => [
        {
          name => 'string'
        },
        {
          name => 'iostream'
        },
        {
          name => 'vector'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Reader_Dynspec_Part.h',
          ref => '_reader___dynspec___part_8h'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/DynspecPart.cpp',
          ref => '_dynspec_part_8cpp'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Reader_Dynspec_Part.cpp',
          ref => '_reader___dynspec___part_8cpp'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Stock_Write_Dynspec_Metadata_Part.cpp',
          ref => '_stock___write___dynspec___metadata___part_8cpp'
        }
      ],
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Root_Metadata_Part.cpp',
      includes => [
        {
          name => 'iostream'
        },
        {
          name => 'string'
        },
        {
          name => 'Stock_Write_Root_Metadata_Part.h',
          ref => '_stock___write___root___metadata___part_8h'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
      ],
      brief => {},
      detailed => {}
    },
    {
      name => 'Stock_Write_Root_Metadata_Part.h',
      includes => [
        {
          name => 'string'
        },
        {
          name => 'iostream'
        },
        {
          name => 'vector'
        },
        {
          name => 'dal/lofar/BF_File.h'
        }
      ],
      included_by => [
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Reader_Root_Part.h',
          ref => '_reader___root___part_8h'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/DynspecPart.cpp',
          ref => '_dynspec_part_8cpp'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Reader_Root_Part.cpp',
          ref => '_reader___root___part_8cpp'
        },
        {
          name => '/home/vilchez/Bureau/dynspec/src/ICD3-ICD6-Rebin/Stock_Write_Root_Metadata_Part.cpp',
          ref => '_stock___write___root___metadata___part_8cpp'
        }
      ],
      brief => {},
      detailed => {}
    }
  ],
  groups => [
  ],
  pages => [
  ]
};
1;
