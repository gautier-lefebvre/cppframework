#ifndef    __LIBRARY_TOOL_ARGUMENTS_HH__
#define    __LIBRARY_TOOL_ARGUMENTS_HH__

#include  <string>
#include  <vector>
#include  <list>
#include  <functional>

namespace fwk {
  /**
   *  \class Arguments Library/Tool/Arguments.hh
   *  \brief Parses the program arguments.
   */
  class Arguments {
  public:

    /**
     *  \struct Arg Library/Tool/Arguments.hh
     *  \brief Structure which contains an argument's flag and list of values.
     */
    struct Arg {
      std::string flag;  /*!< flag of the argument (ex: \a -port). */
      std::vector<std::string> values;  /*!< list of values. */

      /**
       *  \brief Constructor of Arg.
       *
       *  Sets the values to an empty vector.
       *
       *  \param  flag  flag of the argument.
       */
      Arg(const std::string& flag);

      /**
       *  \brief Constructor of Arg.
       *
       *  Copies the values given as parameter.
       *
       *  \param flag flag of the argument.
       *  \param values values of the argument.
       */
      Arg(const std::string& flag, const std::vector<std::string>& values);

      /**
       *  \brief Gets a value at a given index.
       *  \exception std::out_of_range thrown in case the index is not valid.
       *  \param idx index of the value to get.
       *  \return a constant reference on the value.
       */
      const std::string&  operator[](size_t idx) const;
    };

    /**
     *  \struct Expected   Library/Tool/Arguments.hh
     *  \brief Structure which contains the information of an argument that is expected by the program.
     */
    struct Expected {
      std::string                 flag; /*!< flag of the argument. */
      std::string                 type; /*!< type of the argument as a string. */
      std::string                  description; /*!< short description of the argument. */
      bool                        mandatory; /*!< \a true if the argument must be given. */
      std::function<bool (const std::string&)>  typechecker; /*!< a function which tells if the argument is valid (\a nullptr if no verification). */
      size_t                      nbmin; /*!< the minimum number of values of the argument. */
      ssize_t                     nbmax; /*!< the maximum number of values of the argument. Value must be >= to \a nbmin, or \a -1 if no limit. */

      /**
       *  \brief Constructor of Expected argument.
       *
       *  \param flag flag of the argument.
       *  \param type type of the argument as a string.
       *  \param description short description of the argument. Will be used to print the usage.
       *  \param mandatory \a true if the absence of the argument must stop the program.
       *  \param typechecker a function which checks if the argument is valid, or \a nullptr for no verification.
       *  \param nbmin minimum number of values of the argument.
       *  \param nbmax maximal number of values of the argument. Must be >= to \a nbmin or set to \a -1 if no limit.
       */
      Expected(const std::string& flag, const std::string& type, const std::string& description, bool mandatory = false, const std::function<bool (const std::string&)>& typechecker = nullptr, size_t nbmin = 1, ssize_t nbmax = 1);
    };

    /**
     *  \struct Incompatible   Library/Tool/Arguments.hh
     *  \brief Structure which contains a set of incompatible flags.
     */
    struct Incompatible {
      std::list<std::string>  flags; /*!< list of flags that are mutually incompatible. */
      bool          oneMandatory; /*!< \a true if one of the flags must be given. */

      /**
       *  \brief Constructor of Incompatible arguments.
       *
       *  \param flags list of flags that are mutually incompatible.
       *  \param mandatory \a true if one of the flags must be present in the command line.
       */
      Incompatible(const std::list<std::string>& flags, bool mandatory);
    };

  private:
    int                      _ac; /*!< number of arguments of the program. */
    char**                   _av; /*!< unparsed arguments of the program. */
    std::list<Arg*>          _arguments; /*!< parsed arguments of the program. */
    std::list<Arg*>          _defaultValues; /*!< default values to give to arguments if not present. */
    std::list<Expected*>     _expected;  /*!< list of expected arguments of the program. Arguments which are given in the arguments list but are not expected will be ignored. */
    std::list<Incompatible*> _incompatibleArguments; /*!< list of incompatible list of flags. */

  public:
    /**
     *  \brief Constructor of Arguments.
     */
    Arguments(void);

    /**
     *  \brief Destructor of Arguments.
     */
    virtual ~Arguments(void);

  public:
    /**
     *  \brief Inits the class.
     *
     *  Sets the number and list of arguments of the program which will be parsed.
     *
     *  \param ac number of arguments of the program.
     *  \param av arguments of the program.
     */
    void  init(int ac, char **av);

    /**
     *  \brief Adds a default value to an argument.
     *
     *  Adds a default value to an argument, which is overwritten if the argument is given to the program.
     *
     *  \param flag flag of the argument.
     *  \param value value to add to the list of values of the argument.
     */
    void  defaultValue(const std::string& flag, const std::string& value);

    /**
     *  \brief Adds an expected argument.
     *
     *  \param expected an expected argument.
     */
    void  expected(Expected *expected);

    /**
     *  \brief Adds a expected argument.
     *
     *  The expected argument is created then added to the list of expected arguments.
     *  
     *  \param flag flag of the argument.
     *  \param type type of the argument as a string.
     *  \param description short description of the argument. Will be used to print the usage.
     *  \param mandatory \a true if the absence of the argument must stop the program.
     *  \param typechecker a function which checks if the argument is valid, or \a nullptr for no verification.
     *  \param nbmin minimum number of values of the argument.
     *  \param nbmax maximal number of values of the argument. Must be >= to \a nbmin or set to \a -1 if no limit.
     */
    void  expected(const std::string& flag, const std::string& type, const std::string& description, bool mandatory = false, const std::function<bool (const std::string&)>& typechecker = nullptr, size_t nbmin = 1, ssize_t nbmax = 1);

    /**
     *  \brief Adds a list of incompatible flags.
     *
     *  \exception Exception thrown if one of the flag is not in the expected list.
     *  \param incompatible Incompatible flags.
     */
    void  incompatible(Incompatible* incompatible);

    /**
     *  \brief Adds a list of incompatible flags.
     *
     *  \exception Exception thrown if one of the flag is not in the expected list.
     *  \param flags list of flags that are mutually incompatible.
     *  \param mandatory \a true if one of the flags must be present in the command line.
     */
    void  incompatible(const std::list<std::string>& flags, bool mandatory);

    /**
     *  \brief Parses the arguments and checks their validity.
     *
     *  \return \a true if the arguments are valid.
     */
    bool  run(void);

    /**
     *  \brief Gets an argument.
     *  \exception std::out_of_range thrown if the argument is not present.
     *  \param flag flag of the argument to return.
     *  \return a constant pointer on the argument.
     */
    const Arg*  arg(const std::string& flag) const;

    /**
     *  \brief Gets an argument.
     *  \exception std::out_of_range thrown if the argument is not present.
     *  \param flag flag of the argument to return.
     *  \return a constant reference on the argument.
     */
    const Arg&  operator[](const std::string& flag) const;

    /**
     *  \brief Usage of the program.
     *  \return a string describing the usage of the program.
     */
    const std::string  usage(void) const;

  private:
    /**
     *  \brief Checks the validity of the arguments.
     *  \return \a true if the arguments are valid.
     */
    bool  check(void);

    /**
     *  \brief Finds an argument.
     *
     *  Finds an argument and may create it if it is not found.
     *
     *  \param args the arguments list to search in.
     *  \param flag flag of the argument to find.
     *  \param create \a true if the argument must be created if it is not found.
     *  \return a pointer on the Arg instance, or \a nullptr if not found and \a create is \a false.
     */
    Arg* find(std::list<Arg*>& args, const std::string& flag, bool create);

    /**
     *  \brief Finds an argument.
     *  \param args the arguments list to search in.
     *  \param flag flag of the argument to find.
     *  \return a constant pointer on the Arg instance, or \a nullptr if not found.
     */
    const Arg* find(const std::list<Arg*>& args, const std::string& flag) const;

    /**
     *  \brief Checks if a flag is expected.
     *  \param flag flag of the argument.
     *  \return \a true if the argument is expected.
     */
    bool isExpected(const std::string& flag) const;

    /**
     *  \brief Checks if a flag is part of an incompatible set of flags.
     *  \param flag flag of the argument.
     *  \return \a true if the argument is part of an incompatible set of flags.
     */
    bool isIncompatible(const std::string& flag) const;

    /**
     *  \brief Finds an expected argument from its flag.
     *  \param flag flag of the argument.
     *  \return a constant pointer on the expected argument, or \a nullptr if it is not found.
     */
    const Expected* getExpected(const std::string& flag) const;
  };
}

#endif    /* __LIBRARY_TOOL_ARGUMENTS_HH__ */
